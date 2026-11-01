#pragma once

#include "report/IReportSection.hpp"

class DirectoryTreeSection final : public IReportSection
{
public:
	void Write(const ReportContext& context, MarkdownFormatter& formatter) const override;
};
