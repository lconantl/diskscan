#pragma once

#include "report/ReportContext.hpp"
#include "utils/formatter/MarkdownFormatter.hpp"

class IReportSection
{
public:
	virtual ~IReportSection() = default;
	virtual void Write(const ReportContext& context, MarkdownFormatter& formatter) const = 0;
};
