#pragma once

#include "IReportSection.hpp"
#include "ReportContext.hpp"
#include <memory>
#include <string>
#include <vector>

class ReportBuilder
{
public:
	ReportBuilder();
	~ReportBuilder();

	[[nodiscard]] std::string Build(const ReportContext& context) const;

private:
	std::vector<std::unique_ptr<IReportSection>> m_sections;
};
