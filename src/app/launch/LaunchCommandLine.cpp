#include "LaunchCommandLine.hpp"
#include "utils/cli/Cli.hpp"
#include <sstream>

namespace
{
using LaunchDispatcher = cli::FlagDispatcher<LaunchOptions>;

LaunchDispatcher CreateDispatcher()
{
	LaunchDispatcher dispatcher;

	dispatcher.BindSwitch(
		{'h', "help", "Показать эту справку и выйти"}, &LaunchOptions::isHelpRequested);

	return dispatcher;
}

void AssertHasAtMostOneDrive(const std::vector<std::string>& positionals)
{
	if (positionals.size() > 1)
	{
		throw cli::UsageError("Ожидается не более одной буквы диска");
	}
}

std::optional<std::string> ResolveDriveInput(const std::vector<std::string>& positionals)
{
	AssertHasAtMostOneDrive(positionals);

	if (positionals.empty())
	{
		return std::nullopt;
	}

	return positionals.front();
}
} // namespace

LaunchOptions ParseLaunchOptions(const std::vector<std::string>& arguments)
{
	LaunchOptions options;

	const std::vector<std::string> positionals = CreateDispatcher().Dispatch(arguments, options);
	options.driveInput = ResolveDriveInput(positionals);

	return options;
}

std::string BuildHelpMessage()
{
	std::ostringstream stream;

	stream << "DiskScan - CLI утилита для сбора информации о заполненности диска" << std::endl
		   << std::endl
		   << "Использование:" << std::endl
		   << "  diskscan [буква_диска] [опции]" << std::endl
		   << std::endl
		   << "Опции:" << std::endl
		   << CreateDispatcher().BuildHelp() << std::endl
		   << "Примеры:" << std::endl
		   << "  diskscan       Спросит букву диска и соберёт отчёт" << std::endl
		   << "  diskscan C     Собрать отчёт по диску C:" << std::endl
		   << "  diskscan D:    Собрать отчёт по диску D:" << std::endl
		   << std::endl
		   << "Отчёт сохраняется рядом с программой в файл disk-report-<буква>.md" << std::endl;

	return stream.str();
}
