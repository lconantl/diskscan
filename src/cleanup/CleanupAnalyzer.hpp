#pragma once

#include "CleanupRule.hpp"
#include "scan/DirectoryNode.hpp"
#include <filesystem>
#include <vector>

std::vector<CleanupFinding> AnalyzeCleanup(
	const DirectoryNode& tree, const std::filesystem::path& driveRoot);
