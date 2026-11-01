#pragma once

#include "report/IReportSection.hpp"

class LargestFilesSection final : public IReportSection
{
public:
	void Write(const ReportContext& context, MarkdownFormatter& formatter) const override;
};
