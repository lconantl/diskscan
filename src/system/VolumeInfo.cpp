#include "VolumeInfo.hpp"
#include "utils/text/TextConversion.hpp"
#include <array>
#include <stdexcept>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
constexpr auto UNKNOWN_VALUE = "неизвестно";

void AssertIsSpaceRead(const std::error_code& error, const std::filesystem::path& root)
{
	if (error)
	{
		throw std::runtime_error("Не удалось получить размер диска " + ToUtf8(root));
	}
}

void FillSpace(VolumeInfo& info, const std::filesystem::path& root)
{
	std::error_code error;
	const std::filesystem::space_info space = std::filesystem::space(root, error);
	AssertIsSpaceRead(error, root);

	info.totalBytes = space.capacity;
	info.freeBytes = space.free;
}

#ifdef _WIN32
constexpr DWORD NAME_BUFFER_SIZE = MAX_PATH + 1;

std::string DescribeDriveType(const UINT type)
{
	switch (type)
	{
	case DRIVE_FIXED: return "Локальный диск";
	case DRIVE_REMOVABLE: return "Съёмный диск";
	case DRIVE_REMOTE: return "Сетевой диск";
	case DRIVE_CDROM: return "Оптический привод";
	case DRIVE_RAMDISK: return "RAM-диск";
	default: return UNKNOWN_VALUE;
	}
}

std::string ToUtf8Buffer(const wchar_t* buffer)
{
	return ToUtf8(std::filesystem::path(std::wstring(buffer)));
}

void FillVolumeNames(VolumeInfo& info, const std::filesystem::path& root)
{
	std::array<wchar_t, NAME_BUFFER_SIZE> label{};
	std::array<wchar_t, NAME_BUFFER_SIZE> fileSystem{};

	const BOOL success = GetVolumeInformationW(root.c_str(),
		label.data(),
		NAME_BUFFER_SIZE,
		nullptr,
		nullptr,
		nullptr,
		fileSystem.data(),
		NAME_BUFFER_SIZE);

	if (success != 0)
	{
		info.label = ToUtf8Buffer(label.data());
		info.fileSystem = ToUtf8Buffer(fileSystem.data());
	}
}
#endif
} // namespace

VolumeInfo ReadVolumeInfo(const std::filesystem::path& root)
{
	VolumeInfo info{UNKNOWN_VALUE, UNKNOWN_VALUE, UNKNOWN_VALUE};
	FillSpace(info, root);

#ifdef _WIN32
	FillVolumeNames(info, root);
	info.driveType = DescribeDriveType(GetDriveTypeW(root.c_str()));
#endif

	return info;
}

std::uint64_t GetUsedBytes(const VolumeInfo& volume) noexcept
{
	return volume.totalBytes > volume.freeBytes ? volume.totalBytes - volume.freeBytes : 0;
}
