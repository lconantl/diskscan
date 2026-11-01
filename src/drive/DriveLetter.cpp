#include "DriveLetter.hpp"
#include "utils/text/TextConversion.hpp"
#include <array>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
using LayoutPair = std::pair<std::string_view, char>;

constexpr std::array<LayoutPair, 52> CYRILLIC_LAYOUT = {{
	{"ф", 'A'},
	{"и", 'B'},
	{"с", 'C'},
	{"в", 'D'},
	{"у", 'E'},
	{"а", 'F'},
	{"п", 'G'},
	{"р", 'H'},
	{"ш", 'I'},
	{"о", 'J'},
	{"л", 'K'},
	{"д", 'L'},
	{"ь", 'M'},
	{"т", 'N'},
	{"щ", 'O'},
	{"з", 'P'},
	{"й", 'Q'},
	{"к", 'R'},
	{"ы", 'S'},
	{"е", 'T'},
	{"г", 'U'},
	{"м", 'V'},
	{"ц", 'W'},
	{"ч", 'X'},
	{"н", 'Y'},
	{"я", 'Z'},
	{"Ф", 'A'},
	{"И", 'B'},
	{"С", 'C'},
	{"В", 'D'},
	{"У", 'E'},
	{"А", 'F'},
	{"П", 'G'},
	{"Р", 'H'},
	{"Ш", 'I'},
	{"О", 'J'},
	{"Л", 'K'},
	{"Д", 'L'},
	{"Ь", 'M'},
	{"Т", 'N'},
	{"Щ", 'O'},
	{"З", 'P'},
	{"Й", 'Q'},
	{"К", 'R'},
	{"Ы", 'S'},
	{"Е", 'T'},
	{"Г", 'U'},
	{"М", 'V'},
	{"Ц", 'W'},
	{"Ч", 'X'},
	{"Н", 'Y'},
	{"Я", 'Z'},
}};

bool IsSeparator(const char symbol)
{
	return symbol == ':' || symbol == '\\' || symbol == '/';
}

std::string StripSeparators(std::string text)
{
	while (!text.empty() && IsSeparator(text.back()))
	{
		text.pop_back();
	}

	return text;
}

bool IsAsciiLetter(const char symbol)
{
	return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
}

char ToUpperAscii(const char symbol)
{
	if (symbol >= 'a' && symbol <= 'z')
	{
		return static_cast<char>(symbol - 'a' + 'A');
	}

	return symbol;
}

std::optional<char> MapAsciiLetter(const std::string& text)
{
	if (text.size() == 1 && IsAsciiLetter(text.front()))
	{
		return ToUpperAscii(text.front());
	}

	return std::nullopt;
}

std::optional<char> MapCyrillicLetter(const std::string& text)
{
	for (const auto& [cyrillic, latin] : CYRILLIC_LAYOUT)
	{
		if (text == cyrillic)
		{
			return latin;
		}
	}

	return std::nullopt;
}

void AssertIsInputNotEmpty(const std::string& text)
{
	if (text.empty())
	{
		throw std::invalid_argument("Буква диска не указана");
	}
}

char AssertIsDriveLetter(const std::optional<char>& letter, const std::string& input)
{
	if (!letter.has_value())
	{
		throw std::invalid_argument(
			"Некорректная буква диска: \"" + input + "\". Ожидается, например, C или D:");
	}

	return *letter;
}

#ifdef _WIN32
bool IsDrivePresent(const std::filesystem::path& root)
{
	const UINT type = GetDriveTypeW(root.c_str());
	return type != DRIVE_UNKNOWN && type != DRIVE_NO_ROOT_DIR;
}
#else
bool IsDrivePresent(const std::filesystem::path& root)
{
	std::error_code error;
	return std::filesystem::is_directory(root, error);
}
#endif

void AssertIsDrivePresent(const std::filesystem::path& root)
{
	if (!IsDrivePresent(root))
	{
		throw std::runtime_error("Диск " + ToUtf8(root) + " не найден в системе");
	}
}
} // namespace

char ParseDriveLetter(const std::string& input)
{
	const std::string text = StripSeparators(Trim(input));
	AssertIsInputNotEmpty(text);

	std::optional<char> letter = MapAsciiLetter(text);
	if (!letter.has_value())
	{
		letter = MapCyrillicLetter(text);
	}

	return AssertIsDriveLetter(letter, input);
}

std::filesystem::path BuildDriveRoot(const char letter)
{
	return {std::string(1, letter) + ":\\"};
}

std::filesystem::path ResolveDriveRoot(const char letter)
{
	std::filesystem::path root = BuildDriveRoot(letter);
	AssertIsDrivePresent(root);

	return root;
}
