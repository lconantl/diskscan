#include "TextConversion.hpp"
#include <algorithm>
#include <cwctype>
#include <type_traits>

namespace
{
constexpr auto WHITESPACE = " \t\r\n";

template <typename Symbol> Symbol LowerSymbol(const Symbol symbol)
{
	if constexpr (std::is_same_v<Symbol, wchar_t>)
	{
		return static_cast<wchar_t>(std::towlower(static_cast<std::wint_t>(symbol)));
	}
	else
	{
		return (symbol >= 'A' && symbol <= 'Z') ? static_cast<Symbol>(symbol - 'A' + 'a') : symbol;
	}
}
} // namespace

std::string ToUtf8(const std::filesystem::path& path)
{
	const std::u8string text = path.u8string();
	return {text.begin(), text.end()};
}

std::filesystem::path FromUtf8(const std::string& text)
{
	return {std::u8string(text.begin(), text.end())};
}

std::filesystem::path::string_type ToComparisonKey(const std::filesystem::path& path)
{
	std::filesystem::path::string_type key = path.native();
	std::transform(
		key.begin(), key.end(), key.begin(), [](const auto symbol) { return LowerSymbol(symbol); });

	return key;
}

std::string Trim(const std::string& text)
{
	const std::size_t first = text.find_first_not_of(WHITESPACE);
	if (first == std::string::npos)
	{
		return {};
	}

	const std::size_t last = text.find_last_not_of(WHITESPACE);
	return text.substr(first, last - first + 1);
}
