#include "ExecutablePath.hpp"
#include <system_error>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
std::filesystem::path FallbackDirectory()
{
	std::error_code error;
	std::filesystem::path current = std::filesystem::current_path(error);

	return error ? std::filesystem::path(".") : current;
}

#ifdef _WIN32
constexpr DWORD INITIAL_PATH_CAPACITY = MAX_PATH;
constexpr DWORD MAX_PATH_CAPACITY = 32768;

std::filesystem::path ReadModulePath()
{
	for (DWORD capacity = INITIAL_PATH_CAPACITY; capacity <= MAX_PATH_CAPACITY; capacity *= 2)
	{
		std::vector<wchar_t> buffer(capacity);
		const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), capacity);
		if (length > 0 && length < capacity)
		{
			return {std::wstring(buffer.data(), length)};
		}
	}

	return {};
}
#else
std::filesystem::path ReadModulePath()
{
	std::error_code error;
	std::filesystem::path path = std::filesystem::read_symlink("/proc/self/exe", error);

	return error ? std::filesystem::path() : path;
}
#endif
} // namespace

std::filesystem::path GetExecutableDirectory()
{
	const std::filesystem::path modulePath = ReadModulePath();
	if (modulePath.empty())
	{
		return FallbackDirectory();
	}

	return modulePath.parent_path();
}
