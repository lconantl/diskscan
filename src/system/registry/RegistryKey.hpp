#pragma once

#ifdef _WIN32

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <windows.h>

class RegistryKey final
{
public:
	static std::optional<RegistryKey> Open(HKEY root, const std::wstring& subKey, REGSAM viewFlag);

	~RegistryKey() noexcept;

	RegistryKey(const RegistryKey&) = delete;
	RegistryKey& operator=(const RegistryKey&) = delete;
	RegistryKey(RegistryKey&& other) noexcept;
	RegistryKey& operator=(RegistryKey&& other) noexcept;

	[[nodiscard]] std::optional<std::wstring> ReadString(const std::wstring& name) const;
	[[nodiscard]] std::optional<std::uint32_t> ReadDword(const std::wstring& name) const;
	[[nodiscard]] std::vector<std::wstring> ListSubKeys() const;

private:
	explicit RegistryKey(HKEY handle) noexcept;

	void Close() noexcept;

	HKEY m_handle;
};

#endif
