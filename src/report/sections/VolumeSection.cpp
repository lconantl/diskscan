#include "VolumeSection.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"

namespace
{
std::string DescribeShare(const std::uint64_t part, const std::uint64_t total)
{
	return FormatBytes(part) + " (" + FormatPercent(part, total) + ")";
}

std::vector<TableRow> BuildVolumeRows(const VolumeInfo& volume)
{
	return {
		{"Метка", volume.label.empty() ? "—" : volume.label},
		{"Файловая система", volume.fileSystem},
		{"Тип", volume.driveType},
		{"Объём", FormatBytes(volume.totalBytes)},
		{"Занято", DescribeShare(GetUsedBytes(volume), volume.totalBytes)},
		{"Свободно", DescribeShare(volume.freeBytes, volume.totalBytes)},
	};
}

TableRow BuildDifferenceRow(const std::uint64_t usedBytes, const std::uint64_t scannedBytes)
{
	if (usedBytes >= scannedBytes)
	{
		return {"Неучтено сканированием",
			FormatBytes(usedBytes - scannedBytes)
				+ " — System Volume Information, точки восстановления, MFT, закрытые папки"};
	}

	return {"Просканировано больше занятого",
		FormatBytes(scannedBytes - usedBytes) + " — жёсткие ссылки (WinSxS) и сжатие NTFS"};
}

std::vector<TableRow> BuildScanRows(const ReportContext& context)
{
	const ScanTotals& totals = context.scan.totals;

	return {
		{"Просканировано данных", FormatBytes(totals.bytes)},
		{"Файлов", FormatCount(totals.files)},
		{"Папок", FormatCount(totals.directories)},
		{"Облачные файлы (не занимают места)",
			FormatCount(totals.cloudOnlyFiles) + " шт., " + FormatBytes(totals.cloudOnlyBytes)},
		{"Пропущено ссылок и junction", FormatCount(totals.skippedLinks)},
		{"Недоступных папок", FormatCount(totals.inaccessibleDirectories)},
		BuildDifferenceRow(GetUsedBytes(context.volume), totals.bytes),
	};
}
} // namespace

void VolumeSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Диск " + ToUtf8(context.scan.root));
	formatter.AddTable({"Параметр", "Значение"}, BuildVolumeRows(context.volume));

	formatter.AddHeader(3, "Учёт пространства");
	formatter.AddTable({"Параметр", "Значение"}, BuildScanRows(context));
}
