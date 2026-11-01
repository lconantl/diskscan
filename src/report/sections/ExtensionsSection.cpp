#include "ExtensionsSection.hpp"
#include "utils/format/ValueFormat.hpp"

namespace
{
std::vector<TableRow> BuildRows(
	const std::vector<ExtensionRecord>& records, const std::uint64_t totalBytes)
{
	std::vector<TableRow> rows;
	rows.reserve(records.size());

	for (const auto& record : records)
	{
		rows.push_back({record.extension,
			FormatBytes(record.bytes),
			FormatPercent(record.bytes, totalBytes),
			FormatCount(record.files)});
	}

	return rows;
}
} // namespace

void ExtensionsSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Типы файлов");
	formatter.AddTable({"Расширение", "Размер", "Доля", "Файлов"},
		BuildRows(context.scan.extensions, context.scan.totals.bytes));
}
