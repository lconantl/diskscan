#include "ProgramsSection.hpp"
#include "scan/lookup/TreeLookup.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>

namespace
{
constexpr std::size_t ROW_LIMIT = 40;

struct ProgramUsage
{
	const InstalledProgram* program;
	std::optional<std::uint64_t> diskBytes;
};

bool IsOnDrive(const std::filesystem::path& location, const std::filesystem::path& driveRoot)
{
	return !location.empty()
		&& ToComparisonKey(location.root_name()) == ToComparisonKey(driveRoot.root_name());
}

std::optional<std::uint64_t> MeasureOnDisk(const InstalledProgram& program,
	const DirectoryNode& tree,
	const std::filesystem::path& driveRoot)
{
	if (!IsOnDrive(program.location, driveRoot))
	{
		return std::nullopt;
	}

	const DirectoryNode* node = FindDescendant(tree, program.location.relative_path());
	if (node == nullptr)
	{
		return std::nullopt;
	}

	return node->totalBytes;
}

std::uint64_t GetWeight(const ProgramUsage& usage)
{
	return std::max(usage.program->estimatedBytes, usage.diskBytes.value_or(0));
}

bool IsHeavier(const ProgramUsage& left, const ProgramUsage& right)
{
	return GetWeight(left) > GetWeight(right);
}

std::vector<ProgramUsage> CollectUsages(const ReportContext& context)
{
	std::vector<ProgramUsage> usages;
	for (const auto& program : context.programs)
	{
		usages.push_back({&program, MeasureOnDisk(program, *context.scan.tree, context.scan.root)});
	}

	std::sort(usages.begin(), usages.end(), IsHeavier);
	if (usages.size() > ROW_LIMIT)
	{
		usages.resize(ROW_LIMIT);
	}

	return usages;
}

std::string DescribeOptionalBytes(const std::optional<std::uint64_t>& bytes)
{
	return bytes.has_value() ? FormatBytes(*bytes) : "—";
}

std::string DescribeEstimate(const std::uint64_t bytes)
{
	return bytes > 0 ? FormatBytes(bytes) : "—";
}

std::string DescribeLocation(const std::filesystem::path& location)
{
	return location.empty() ? "—" : "`" + ToUtf8(location) + "`";
}

TableRow BuildRow(const ProgramUsage& usage)
{
	const InstalledProgram& program = *usage.program;

	return {program.name,
		program.publisher.empty() ? "—" : program.publisher,
		DescribeEstimate(program.estimatedBytes),
		DescribeOptionalBytes(usage.diskBytes),
		DescribeLocation(program.location)};
}

std::vector<TableRow> BuildRows(const std::vector<ProgramUsage>& usages)
{
	std::vector<TableRow> rows;
	rows.reserve(usages.size());

	for (const auto& usage : usages)
	{
		rows.push_back(BuildRow(usage));
	}

	return rows;
}
} // namespace

void ProgramsSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Установленные программы");

	if (context.programs.empty())
	{
		formatter.AddText("Список программ недоступен.");
		return;
	}

	formatter.AddTable({"Программа", "Издатель", "Размер (реестр)", "Размер на диске", "Папка"},
		BuildRows(CollectUsages(context)));
}
