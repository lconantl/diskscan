#include "SystemSection.hpp"
#include "utils/format/ValueFormat.hpp"

namespace
{
std::string DescribeElevation(const bool isElevated)
{
	return isElevated ? "Да" : "Нет (часть системных папок могла быть пропущена)";
}

std::vector<TableRow> BuildRows(const ReportContext& context)
{
	const SystemInfo& system = context.system;

	return {
		{"Дата отчёта", FormatCurrentDateTime()},
		{"ОС", system.osName},
		{"Версия", system.osVersion},
		{"Сборка", system.osBuild},
		{"Оперативная память", FormatBytes(system.memoryBytes)},
		{"Системный диск", system.systemDrive},
		{"Запуск от администратора", DescribeElevation(system.isElevated)},
		{"Время сканирования", FormatDuration(context.scan.totals.duration)},
	};
}
} // namespace

void SystemSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Система");
	formatter.AddTable({"Параметр", "Значение"}, BuildRows(context));
}
