#include "CommandLine.hpp"
#include "utils/cli/Cli.hpp"

#ifdef _WIN32
#include "utils/text/TextConversion.hpp"
#include <memory>
#include <windows.h>

#include <shellapi.h>
#endif

namespace
{
std::vector<std::string> ReadNarrowArguments(const int argc, char* argv[])
{
	return cli::ReadArguments(argc, argv);
}

#ifdef _WIN32
struct LocalMemoryDeleter
{
	void operator()(LPWSTR* memory) const noexcept
	{
		LocalFree(memory);
	}
};

using WideArguments = std::unique_ptr<LPWSTR[], LocalMemoryDeleter>;

std::vector<std::string> ConvertToUtf8(LPWSTR* wideArguments, const int count)
{
	std::vector<std::string> arguments;
	arguments.reserve(static_cast<std::size_t>(count));

	for (int index = 0; index < count; ++index)
	{
		arguments.push_back(ToUtf8(std::filesystem::path(std::wstring(wideArguments[index]))));
	}

	return arguments;
}

std::vector<const char*> BuildPointers(const std::vector<std::string>& arguments)
{
	std::vector<const char*> pointers;
	pointers.reserve(arguments.size());

	for (const auto& argument : arguments)
	{
		pointers.push_back(argument.c_str());
	}

	return pointers;
}

std::vector<std::string> ReadWideArguments(const int argc, char* argv[])
{
	int count = 0;
	const WideArguments wideArguments(CommandLineToArgvW(GetCommandLineW(), &count));
	if (wideArguments == nullptr || count <= 0)
	{
		return ReadNarrowArguments(argc, argv);
	}

	const std::vector<std::string> utf8Arguments = ConvertToUtf8(wideArguments.get(), count);
	const std::vector<const char*> pointers = BuildPointers(utf8Arguments);

	return cli::ReadArguments(count, pointers.data());
}
#endif
} // namespace

std::vector<std::string> ReadCommandLineArguments(const int argc, char* argv[])
{
#ifdef _WIN32
	return ReadWideArguments(argc, argv);
#else
	return ReadNarrowArguments(argc, argv);
#endif
}
