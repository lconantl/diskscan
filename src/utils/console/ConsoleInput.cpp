#include "ConsoleInput.hpp"
#include "utils/text/TextConversion.hpp"
#include <array>
#include <iostream>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
std::string ReadStandardLine()
{
	std::string line;
	std::getline(std::cin, line);

	return Trim(line);
}

#ifdef _WIN32
constexpr DWORD CONSOLE_BUFFER_SIZE = 256;

std::optional<std::wstring> ReadNativeConsoleLine()
{
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	if (input == INVALID_HANDLE_VALUE || GetConsoleMode(input, &mode) == 0)
	{
		return std::nullopt;
	}

	std::array<wchar_t, CONSOLE_BUFFER_SIZE> buffer{};
	DWORD readCount = 0;
	if (ReadConsoleW(input, buffer.data(), CONSOLE_BUFFER_SIZE, &readCount, nullptr) == 0)
	{
		return std::nullopt;
	}

	return std::wstring(buffer.data(), readCount);
}
#endif
} // namespace

std::string ReadConsoleLine()
{
#ifdef _WIN32
	if (const std::optional<std::wstring> line = ReadNativeConsoleLine())
	{
		return Trim(ToUtf8(std::filesystem::path(*line)));
	}
#endif

	return ReadStandardLine();
}

void WaitForEnter()
{
	std::cout << std::endl << "Нажмите Enter, чтобы закрыть окно..." << std::flush;
	ReadConsoleLine();
}
