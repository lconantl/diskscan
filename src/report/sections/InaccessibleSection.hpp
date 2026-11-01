#pragma once

#include "report/IReportSection.hpp"

class InaccessibleSection final : public IReportSection
{
public:
	void Write(const ReportContext& context, MarkdownFormatter& formatter) const override;
};
