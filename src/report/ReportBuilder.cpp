#include "ReportBuilder.hpp"
#include "report/sections/CleanupSection.hpp"
#include "report/sections/DirectoryTreeSection.hpp"
#include "report/sections/ExtensionsSection.hpp"
#include "report/sections/InaccessibleSection.hpp"
#include "report/sections/LargestFilesSection.hpp"
#include "report/sections/ProgramsSection.hpp"
#include "report/sections/SystemSection.hpp"
#include "report/sections/VolumeSection.hpp"
#include "utils/text/TextConversion.hpp"

namespace
{
std::vector<std::unique_ptr<IReportSection>> CreateSections()
{
	std::vector<std::unique_ptr<IReportSection>> sections;

	sections.push_back(std::make_unique<SystemSection>());
	sections.push_back(std::make_unique<VolumeSection>());
	sections.push_back(std::make_unique<CleanupSection>());
	sections.push_back(std::make_unique<DirectoryTreeSection>());
	sections.push_back(std::make_unique<LargestFilesSection>());
	sections.push_back(std::make_unique<ExtensionsSection>());
	sections.push_back(std::make_unique<ProgramsSection>());
	sections.push_back(std::make_unique<InaccessibleSection>());

	return sections;
}

std::string BuildTitle(const ReportContext& context)
{
	return "Отчёт о заполненности диска " + ToUtf8(context.scan.root);
}
} // namespace

ReportBuilder::ReportBuilder()
	: m_sections(CreateSections())
{
}

ReportBuilder::~ReportBuilder() = default;

std::string ReportBuilder::Build(const ReportContext& context) const
{
	MarkdownFormatter formatter;
	formatter.AddHeader(1, BuildTitle(context));

	for (const auto& section : m_sections)
	{
		section->Write(context, formatter);
	}

	return formatter.Build();
}
