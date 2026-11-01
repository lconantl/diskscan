#include "Application.hpp"
#include "app/launch/LaunchCommandLine.hpp"
#include "cleanup/CleanupAnalyzer.hpp"
#include "drive/DriveLetter.hpp"
#include "report/ReportBuilder.hpp"
#include "scan/DiskScanner.hpp"
#include "utils/console/ConsoleInput.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/process/CommandLine.hpp"
#include "utils/process/ExecutablePath.hpp"
#include "utils/text/TextConversion.hpp"
#include "writer/file/FileOutput.hpp"
#include <iostream>
#include <string>

namespace
{
std::string PromptDriveInput()
{
	std::cout << "Введите букву диска для анализа (например, C): " << std::flush;
	return ReadConsoleLine();
}

std::string ResolveDriveInput(const LaunchOptions& options)
{
	return options.driveInput.has_value() ? *options.driveInput : PromptDriveInput();
}

void PrintElevationWarning(const SystemInfo& system)
{
	if (!system.isElevated)
	{
		std::cout << "[!] Программа запущена без прав администратора." << std::endl
				  << "    Часть системных папок будет недоступна. Для полного отчёта:" << std::endl
				  << "    ПКМ по diskscan.exe -> «Запуск от имени администратора»." << std::endl
				  << std::endl;
	}
}

void PrintVolumeSummary(const VolumeInfo& volume)
{
	std::cout << "Объём: " << FormatBytes(volume.totalBytes)
			  << ", занято: " << FormatBytes(GetUsedBytes(volume))
			  << ", свободно: " << FormatBytes(volume.freeBytes) << std::endl;
}

ScanResult ScanDrive(const std::filesystem::path& root)
{
	std::cout << "Сканирование " << ToUtf8(root) << " (это может занять несколько минут)..."
			  << std::endl;

	return DiskScanner(root).Scan();
}

std::vector<CleanupFinding> AnalyzeDrive(const ScanResult& scan)
{
	std::cout << "Анализ известных мест для очистки..." << std::endl;
	return AnalyzeCleanup(*scan.tree, scan.root);
}

std::vector<InstalledProgram> ReadPrograms()
{
	std::cout << "Чтение списка установленных программ..." << std::endl;
	return ReadInstalledPrograms();
}

std::filesystem::path BuildReportPath(const char letter)
{
	return GetExecutableDirectory() / ("disk-report-" + std::string(1, letter) + ".md");
}

void SaveReport(const std::filesystem::path& path, const std::string& content)
{
	FileOutput(path).Write(content);
}

void PrintSuccessMessage(const std::filesystem::path& path)
{
	std::cout << std::endl
			  << "Готово! Отчёт сохранён в файл:" << std::endl
			  << "  " << ToUtf8(path) << std::endl
			  << "Отправьте этот файл в чат для анализа." << std::endl;
}
} // namespace

Application::Application(const int argc, char* argv[])
	: m_options(ParseLaunchOptions(ReadCommandLineArguments(argc, argv)))
{
}

Application::~Application() = default;

void Application::Run() const
{
	if (m_options.isHelpRequested)
	{
		std::cout << BuildHelpMessage() << std::endl;
		return;
	}

	const char letter = ParseDriveLetter(ResolveDriveInput(m_options));
	const std::filesystem::path root = ResolveDriveRoot(letter);

	ReportContext context;
	context.system = ReadSystemInfo();
	PrintElevationWarning(context.system);

	context.volume = ReadVolumeInfo(root);
	PrintVolumeSummary(context.volume);

	context.scan = ScanDrive(root);
	context.cleanup = AnalyzeDrive(context.scan);
	context.programs = ReadPrograms();

	const std::filesystem::path reportPath = BuildReportPath(letter);
	SaveReport(reportPath, ReportBuilder().Build(context));
	PrintSuccessMessage(reportPath);
}
