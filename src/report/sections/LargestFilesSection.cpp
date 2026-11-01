#include "LargestFilesSection.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"

namespace
{
TableRow BuildRow(const std::size_t index, const FileRecord& record)
{
	return {
		std::to_string(index + 1),
		FormatBytes(record.size),
		FormatUnixDate(record.lastWriteUnixSeconds),
		"`" + ToUtf8(record.path) + "`",
	};
}

std::vector<TableRow> BuildRows(const std::vector<FileRecord>& records)
{
	std::vector<TableRow> rows;
	rows.reserve(records.size());

	for (std::size_t index = 0; index < records.size(); ++index)
	{
		rows.push_back(BuildRow(index, records[index]));
	}

	return rows;
}
} // namespace

void LargestFilesSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Крупнейшие файлы");
	formatter.AddTable({"#", "Размер", "Изменён", "Путь"}, BuildRows(context.scan.largestFiles));
}
