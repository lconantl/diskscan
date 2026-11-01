#include "app/Application.hpp"
#include "app/launch/LaunchCommandLine.hpp"
#include "utils/cli/Cli.hpp"
#include "utils/console/ConsoleEncoding.hpp"
#include "utils/console/ConsoleInput.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

namespace
{
bool IsLaunchedWithoutArguments(const int argc)
{
	return argc <= 1;
}

int RunApplication(const int argc, char* argv[])
{
	try
	{
		const Application app(argc, argv);
		app.Run();
	}
	catch (const cli::UsageError& e)
	{
		std::cerr << "[Ошибка] \t" << e.what() << std::endl;
		std::cerr << BuildHelpMessage() << std::endl;
		return cli::UsageError::EXIT_CODE;
	}
	catch (const std::exception& e)
	{
		std::cerr << std::endl << "[Ошибка] \t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
} // namespace

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding console;
		const int exitCode = RunApplication(argc, argv);

		if (IsLaunchedWithoutArguments(argc))
		{
			WaitForEnter();
		}

		return exitCode;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[Ошибка] \t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
