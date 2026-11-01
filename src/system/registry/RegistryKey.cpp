#ifdef _WIN32

#include "RegistryKey.hpp"
#include <array>
#include <utility>

namespace
{
constexpr DWORD MAX_KEY_NAME_LENGTH = 256;
constexpr DWORD STRING_VALUE_FLAGS = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;

std::optional<DWORD> QueryStringSize(HKEY handle, const std::wstring& name)
{
	DWORD size = 0;
	const LSTATUS status
		= RegGetValueW(handle, nullptr, name.c_str(), STRING_VALUE_FLAGS, nullptr, nullptr, &size);

	if (status != ERROR_SUCCESS || size < sizeof(wchar_t))
	{
		return std::nullopt;
	}

	return size;
}

std::wstring TrimTrailingZeros(std::wstring text)
{
	while (!text.empty() && text.back() == L'\0')
	{
		text.pop_back();
	}

	return text;
}
} // namespace

std::optional<RegistryKey> RegistryKey::Open(HKEY root, const std::wstring& subKey, REGSAM viewFlag)
{
	HKEY handle = nullptr;
	const LSTATUS status = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ | viewFlag, &handle);
	if (status != ERROR_SUCCESS)
	{
		return std::nullopt;
	}

	return RegistryKey(handle);
}

RegistryKey::RegistryKey(HKEY handle) noexcept
	: m_handle(handle)
{
}

RegistryKey::~RegistryKey() noexcept
{
	Close();
}

RegistryKey::RegistryKey(RegistryKey&& other) noexcept
	: m_handle(std::exchange(other.m_handle, nullptr))
{
}

RegistryKey& RegistryKey::operator=(RegistryKey&& other) noexcept
{
	if (this != &other)
	{
		Close();
		m_handle = std::exchange(other.m_handle, nullptr);
	}

	return *this;
}

void RegistryKey::Close() noexcept
{
	if (m_handle != nullptr)
	{
		RegCloseKey(m_handle);
		m_handle = nullptr;
	}
}

std::optional<std::wstring> RegistryKey::ReadString(const std::wstring& name) const
{
	const std::optional<DWORD> size = QueryStringSize(m_handle, name);
	if (!size.has_value())
	{
		return std::nullopt;
	}

	std::wstring buffer(*size / sizeof(wchar_t), L'\0');
	DWORD actualSize = *size;
	const LSTATUS status = RegGetValueW(
		m_handle, nullptr, name.c_str(), STRING_VALUE_FLAGS, nullptr, buffer.data(), &actualSize);

	if (status != ERROR_SUCCESS)
	{
		return std::nullopt;
	}

	return TrimTrailingZeros(std::move(buffer));
}

std::optional<std::uint32_t> RegistryKey::ReadDword(const std::wstring& name) const
{
	DWORD value = 0;
	DWORD size = sizeof(value);
	const LSTATUS status
		= RegGetValueW(m_handle, nullptr, name.c_str(), RRF_RT_REG_DWORD, nullptr, &value, &size);

	if (status != ERROR_SUCCESS)
	{
		return std::nullopt;
	}

	return static_cast<std::uint32_t>(value);
}

std::vector<std::wstring> RegistryKey::ListSubKeys() const
{
	std::vector<std::wstring> names;
	std::array<wchar_t, MAX_KEY_NAME_LENGTH> buffer{};

	for (DWORD index = 0;; ++index)
	{
		DWORD length = MAX_KEY_NAME_LENGTH;
		const LSTATUS status = RegEnumKeyExW(
			m_handle, index, buffer.data(), &length, nullptr, nullptr, nullptr, nullptr);

		if (status != ERROR_SUCCESS)
		{
			break;
		}
		names.emplace_back(buffer.data(), length);
	}

	return names;
}

#endif
