#pragma once

#include "cleanup/CleanupRule.hpp"
#include "scan/ScanResult.hpp"
#include "system/InstalledProgram.hpp"
#include "system/SystemInfo.hpp"
#include "system/VolumeInfo.hpp"
#include <vector>

struct ReportContext
{
	SystemInfo system;
	VolumeInfo volume;
	ScanResult scan;
	std::vector<CleanupFinding> cleanup;
	std::vector<InstalledProgram> programs;
};
