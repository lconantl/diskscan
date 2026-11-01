#include "InstalledProgram.hpp"
#include "utils/text/TextConversion.hpp"
#include <algorithm>
#include <optional>
#include <set>

#ifdef _WIN32
#include "system/registry/RegistryKey.hpp"
#include <windows.h>
#endif

namespace
{
bool IsLargerProgram(const InstalledProgram& left, const InstalledProgram& right)
{
	return left.estimatedBytes > right.estimatedBytes;
}

std::vector<InstalledProgram> RemoveDuplicates(std::vector<InstalledProgram> programs)
{
	std::sort(programs.begin(), programs.end(), IsLargerProgram);

	std::set<std::string> seenNames;
	std::vector<InstalledProgram> unique;
	for (auto& program : programs)
	{
		if (seenNames.insert(program.name).second)
		{
			unique.push_back(std::move(program));
		}
	}

	return unique;
}

#ifdef _WIN32
constexpr auto UNINSTALL_KEY = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
constexpr std::uint64_t BYTES_PER_KILOBYTE = 1024;

struct UninstallSource
{
	HKEY root;
	REGSAM view;
};

std::string ReadText(const RegistryKey& key, const wchar_t* name)
{
	const std::optional<std::wstring> value = key.ReadString(name);
	return value.has_value() ? ToUtf8(std::filesystem::path(*value)) : std::string();
}

bool IsHiddenEntry(const RegistryKey& key)
{
	const bool isSystemComponent = key.ReadDword(L"SystemComponent").value_or(0) == 1;
	const bool isUpdate = key.ReadString(L"ParentKeyName").has_value();

	return isSystemComponent || isUpdate;
}

std::filesystem::path ReadLocation(const RegistryKey& key)
{
	const std::optional<std::wstring> value = key.ReadString(L"InstallLocation");
	if (!value.has_value() || value->empty())
	{
		return {};
	}

	std::wstring location = *value;
	std::erase(location, L'"');

	return {location};
}

std::optional<InstalledProgram> ReadProgram(const RegistryKey& key)
{
	const std::string name = ReadText(key, L"DisplayName");
	if (name.empty() || IsHiddenEntry(key))
	{
		return std::nullopt;
	}

	const std::uint64_t kilobytes = key.ReadDword(L"EstimatedSize").value_or(0);

	return InstalledProgram{
		name, ReadText(key, L"Publisher"), kilobytes * BYTES_PER_KILOBYTE, ReadLocation(key)};
}

void CollectFromSource(const UninstallSource& source, std::vector<InstalledProgram>& programs)
{
	const std::optional<RegistryKey> uninstall
		= RegistryKey::Open(source.root, UNINSTALL_KEY, source.view);
	if (!uninstall.has_value())
	{
		return;
	}

	for (const auto& subKeyName : uninstall->ListSubKeys())
	{
		const std::wstring subKeyPath = std::wstring(UNINSTALL_KEY) + L"\\" + subKeyName;
		const std::optional<RegistryKey> entry
			= RegistryKey::Open(source.root, subKeyPath, source.view);

		if (entry.has_value())
		{
			if (std::optional<InstalledProgram> program = ReadProgram(*entry))
			{
				programs.push_back(std::move(*program));
			}
		}
	}
}

std::vector<InstalledProgram> CollectAllSources()
{
	const std::vector<UninstallSource> sources = {
		{HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY},
		{HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY},
		{HKEY_CURRENT_USER, 0},
	};

	std::vector<InstalledProgram> programs;
	for (const auto& source : sources)
	{
		CollectFromSource(source, programs);
	}

	return programs;
}
#else
std::vector<InstalledProgram> CollectAllSources()
{
	return {};
}
#endif
} // namespace

std::vector<InstalledProgram> ReadInstalledPrograms()
{
	return RemoveDuplicates(CollectAllSources());
}
