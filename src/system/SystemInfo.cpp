#include "SystemInfo.hpp"
#include "utils/text/TextConversion.hpp"
#include <array>
#include <cstdlib>
#include <optional>
#include <string>

#ifdef _WIN32
#include "system/registry/RegistryKey.hpp"
#include "system/token/ProcessToken.hpp"
#include <windows.h>
#endif

namespace
{
constexpr auto UNKNOWN_VALUE = "неизвестно";

#ifdef _WIN32
constexpr DWORD ENVIRONMENT_BUFFER_SIZE = 256;

std::string ReadSystemDrive()
{
	std::array<wchar_t, ENVIRONMENT_BUFFER_SIZE> buffer{};
	const DWORD length
		= GetEnvironmentVariableW(L"SystemDrive", buffer.data(), ENVIRONMENT_BUFFER_SIZE);
	if (length == 0 || length >= ENVIRONMENT_BUFFER_SIZE)
	{
		return UNKNOWN_VALUE;
	}

	return ToUtf8(std::filesystem::path(std::wstring(buffer.data(), length)));
}
#else
std::string ReadSystemDrive()
{
	const char* value = std::getenv("SystemDrive");
	return value != nullptr ? std::string(value) : std::string(UNKNOWN_VALUE);
}
#endif

#ifdef _WIN32
constexpr unsigned long WINDOWS_11_FIRST_BUILD = 22000;
constexpr auto CURRENT_VERSION_KEY = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

std::string ToUtf8Text(const std::optional<std::wstring>& value)
{
	return value.has_value() ? ToUtf8(std::filesystem::path(*value)) : UNKNOWN_VALUE;
}

std::string FixWindows11Name(std::string name, const std::string& build)
{
	const std::string legacyName = "Windows 10";
	const bool isWindows11 = std::strtoul(build.c_str(), nullptr, 10) >= WINDOWS_11_FIRST_BUILD;

	if (isWindows11 && name.starts_with(legacyName))
	{
		name.replace(0, legacyName.size(), "Windows 11");
	}

	return name;
}

std::string BuildFullBuild(const RegistryKey& key)
{
	std::string build = ToUtf8Text(key.ReadString(L"CurrentBuild"));
	if (const std::optional<std::uint32_t> revision = key.ReadDword(L"UBR"))
	{
		build += "." + std::to_string(*revision);
	}

	return build;
}

void FillOsInfo(SystemInfo& info)
{
	const std::optional<RegistryKey> key
		= RegistryKey::Open(HKEY_LOCAL_MACHINE, CURRENT_VERSION_KEY, KEY_WOW64_64KEY);
	if (!key.has_value())
	{
		return;
	}

	info.osBuild = BuildFullBuild(*key);
	info.osVersion = ToUtf8Text(key->ReadString(L"DisplayVersion"));
	info.osName = FixWindows11Name(ToUtf8Text(key->ReadString(L"ProductName")), info.osBuild);
}

std::uint64_t ReadMemoryBytes()
{
	MEMORYSTATUSEX status{};
	status.dwLength = sizeof(status);

	return GlobalMemoryStatusEx(&status) != 0 ? status.ullTotalPhys : 0;
}

#endif
} // namespace

SystemInfo ReadSystemInfo()
{
	SystemInfo info{UNKNOWN_VALUE, UNKNOWN_VALUE, UNKNOWN_VALUE, ReadSystemDrive()};

#ifdef _WIN32
	FillOsInfo(info);
	info.memoryBytes = ReadMemoryBytes();
	info.isElevated = ProcessToken().IsElevated();
#endif

	return info;
}
