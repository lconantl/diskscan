#include "CleanupSection.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"
#include <array>
#include <cstddef>

namespace
{
constexpr std::size_t ROW_LIMIT = 80;
constexpr std::array<CleanupMethod, 4> METHODS
	= {CleanupMethod::Safe, CleanupMethod::SystemTool, CleanupMethod::Review, CleanupMethod::Keep};

TableRow BuildRow(const CleanupFinding& finding)
{
	return {
		FormatBytes(finding.bytes),
		finding.category,
		"`" + ToUtf8(finding.path) + "`",
		FormatCount(finding.files),
		DescribeMethod(finding.method),
		finding.advice,
	};
}

std::vector<TableRow> BuildRows(const std::vector<CleanupFinding>& findings)
{
	std::vector<TableRow> rows;
	for (const auto& finding : findings)
	{
		if (rows.size() >= ROW_LIMIT)
		{
			break;
		}
		rows.push_back(BuildRow(finding));
	}

	return rows;
}

std::uint64_t SumByMethod(const std::vector<CleanupFinding>& findings, const CleanupMethod method)
{
	std::uint64_t total = 0;
	for (const auto& finding : findings)
	{
		if (finding.method == method)
		{
			total += finding.bytes;
		}
	}

	return total;
}

std::vector<std::string> BuildSummary(const std::vector<CleanupFinding>& findings)
{
	std::vector<std::string> items;
	for (const CleanupMethod method : METHODS)
	{
		items.push_back(DescribeMethod(method) + ": " + FormatBytes(SumByMethod(findings, method)));
	}

	return items;
}
} // namespace

void CleanupSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Известные места для очистки");

	if (context.cleanup.empty())
	{
		formatter.AddText("Известных мест для очистки не найдено.");
		return;
	}

	formatter.AddTable({"Размер", "Категория", "Путь", "Файлов", "Как чистить", "Совет"},
		BuildRows(context.cleanup));

	formatter.AddHeader(3, "Сводка по способу очистки");
	formatter.AddBulletList(BuildSummary(context.cleanup));
	formatter.AddText("Суммы приблизительны: некоторые места вложены друг в друга.");
}
