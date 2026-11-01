#include "InaccessibleSection.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"

namespace
{
std::vector<std::string> BuildItems(const std::vector<std::filesystem::path>& paths)
{
	std::vector<std::string> items;
	items.reserve(paths.size());

	for (const auto& path : paths)
	{
		items.push_back("`" + ToUtf8(path) + "`");
	}

	return items;
}

std::string BuildIntro(const ReportContext& context)
{
	std::string intro = "Всего недоступно папок: "
		+ FormatCount(context.scan.totals.inaccessibleDirectories) + ".";

	if (!context.system.isElevated)
	{
		intro += " Запустите программу от администратора для более полного отчёта.";
	}

	return intro;
}
} // namespace

void InaccessibleSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	formatter.AddHeader(2, "Недоступные папки");

	if (context.scan.inaccessiblePaths.empty())
	{
		formatter.AddText("Все папки были доступны для чтения.");
		return;
	}

	formatter.AddText(BuildIntro(context));
	formatter.AddBulletList(BuildItems(context.scan.inaccessiblePaths));
}
