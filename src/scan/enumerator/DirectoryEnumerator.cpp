#include "DirectoryEnumerator.hpp"
#include <chrono>
#include <system_error>

#ifdef _WIN32
#include "FindHandle.hpp"
#include <windows.h>
#endif

namespace
{
#ifdef _WIN32
constexpr DWORD CLOUD_RECALL_ON_DATA_ACCESS = 0x00400000;
constexpr DWORD CLOUD_RECALL_ON_OPEN = 0x00040000;
constexpr DWORD NAME_SURROGATE_BIT = 0x20000000;
constexpr std::int64_t FILETIME_TICKS_PER_SECOND = 10000000;
constexpr std::int64_t FILETIME_UNIX_EPOCH_SECONDS = 11644473600;
constexpr auto LONG_PATH_PREFIX = L"\\\\?\\";

bool HasAttribute(const WIN32_FIND_DATAW& data, const DWORD attribute)
{
	return (data.dwFileAttributes & attribute) != 0;
}

bool IsDotEntry(const WIN32_FIND_DATAW& data)
{
	const std::wstring_view name = data.cFileName;
	return name == L"." || name == L"..";
}

bool IsNameSurrogate(const WIN32_FIND_DATAW& data)
{
	return HasAttribute(data, FILE_ATTRIBUTE_REPARSE_POINT)
		&& (data.dwReserved0 & NAME_SURROGATE_BIT) != 0;
}

bool IsCloudOnly(const WIN32_FIND_DATAW& data)
{
	return HasAttribute(data, CLOUD_RECALL_ON_DATA_ACCESS)
		|| HasAttribute(data, CLOUD_RECALL_ON_OPEN) || HasAttribute(data, FILE_ATTRIBUTE_OFFLINE);
}

std::uint64_t CombineSize(const WIN32_FIND_DATAW& data)
{
	return (static_cast<std::uint64_t>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
}

std::int64_t ToUnixSeconds(const FILETIME& time)
{
	const std::uint64_t ticks
		= (static_cast<std::uint64_t>(time.dwHighDateTime) << 32) | time.dwLowDateTime;

	return static_cast<std::int64_t>(ticks) / FILETIME_TICKS_PER_SECOND
		- FILETIME_UNIX_EPOCH_SECONDS;
}

DirectoryEntry ConvertEntry(const WIN32_FIND_DATAW& data)
{
	DirectoryEntry entry;
	entry.name = std::filesystem::path(data.cFileName);
	entry.isDirectory = HasAttribute(data, FILE_ATTRIBUTE_DIRECTORY);
	entry.isLink = IsNameSurrogate(data);
	entry.isCloudOnly = IsCloudOnly(data);
	entry.size = entry.isDirectory ? 0 : CombineSize(data);
	entry.lastWriteUnixSeconds = ToUnixSeconds(data.ftLastWriteTime);

	return entry;
}

std::wstring WithLongPathPrefix(const std::filesystem::path& path)
{
	const std::wstring& native = path.native();
	if (native.starts_with(LONG_PATH_PREFIX))
	{
		return native;
	}

	return LONG_PATH_PREFIX + native;
}

std::wstring BuildSearchPattern(const std::filesystem::path& directory)
{
	std::wstring pattern = WithLongPathPrefix(directory);
	if (!pattern.ends_with(L'\\'))
	{
		pattern += L'\\';
	}

	return pattern + L'*';
}

bool IsEmptyResult(const DWORD error)
{
	return error == ERROR_FILE_NOT_FOUND || error == ERROR_NO_MORE_FILES;
}

void AppendEntry(DirectoryListing& listing, const WIN32_FIND_DATAW& data)
{
	if (!IsDotEntry(data))
	{
		listing.entries.push_back(ConvertEntry(data));
	}
}

DirectoryListing ListNative(const std::filesystem::path& directory)
{
	FindHandle handle(BuildSearchPattern(directory));
	DirectoryListing listing;

	if (!handle.IsOpen())
	{
		listing.isAccessible = IsEmptyResult(handle.GetOpenError());
		return listing;
	}

	do
	{
		AppendEntry(listing, handle.Current());
	} while (handle.MoveNext());

	return listing;
}

std::optional<DirectoryEntry> FindNative(const std::filesystem::path& path)
{
	const FindHandle handle(WithLongPathPrefix(path));
	if (!handle.IsOpen())
	{
		return std::nullopt;
	}

	return ConvertEntry(handle.Current());
}
#else
std::int64_t ToUnixSeconds(const std::filesystem::file_time_type time)
{
	const auto systemTime = std::chrono::file_clock::to_sys(time);
	return std::chrono::duration_cast<std::chrono::seconds>(systemTime.time_since_epoch()).count();
}

std::uint64_t ReadFileSize(const std::filesystem::directory_entry& item)
{
	std::error_code error;
	const std::uintmax_t size = item.file_size(error);

	return error ? 0 : static_cast<std::uint64_t>(size);
}

std::int64_t ReadLastWrite(const std::filesystem::directory_entry& item)
{
	std::error_code error;
	const std::filesystem::file_time_type time = item.last_write_time(error);

	return error ? 0 : ToUnixSeconds(time);
}

DirectoryEntry ConvertEntry(const std::filesystem::directory_entry& item)
{
	std::error_code error;
	const std::filesystem::file_status status = item.symlink_status(error);

	DirectoryEntry entry;
	entry.name = item.path().filename();
	entry.isLink = std::filesystem::is_symlink(status);
	entry.isDirectory = std::filesystem::is_directory(status);
	entry.size = std::filesystem::is_regular_file(status) ? ReadFileSize(item) : 0;
	entry.lastWriteUnixSeconds = ReadLastWrite(item);

	return entry;
}

DirectoryListing ListNative(const std::filesystem::path& directory)
{
	DirectoryListing listing;
	std::error_code error;
	std::filesystem::directory_iterator iterator(directory, error);

	if (error)
	{
		listing.isAccessible = false;
		return listing;
	}

	for (const std::filesystem::directory_iterator end; iterator != end; iterator.increment(error))
	{
		listing.entries.push_back(ConvertEntry(*iterator));
	}

	return listing;
}

std::optional<DirectoryEntry> FindNative(const std::filesystem::path& path)
{
	std::error_code error;
	const std::filesystem::directory_entry item(path, error);
	if (error || !item.exists(error))
	{
		return std::nullopt;
	}

	return ConvertEntry(item);
}
#endif
} // namespace

DirectoryListing ListDirectory(const std::filesystem::path& directory)
{
	return ListNative(directory);
}

std::optional<DirectoryEntry> FindEntry(const std::filesystem::path& path)
{
	return FindNative(path);
}
