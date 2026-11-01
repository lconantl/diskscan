#pragma once

#include "DirectoryNode.hpp"
#include "scan/collectors/ExtensionStatistics.hpp"
#include "scan/collectors/TopFilesCollector.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

struct ScanTotals
{
	std::uint64_t files = 0;
	std::uint64_t directories = 0;
	std::uint64_t bytes = 0;
	std::uint64_t cloudOnlyFiles = 0;
	std::uint64_t cloudOnlyBytes = 0;
	std::uint64_t skippedLinks = 0;
	std::uint64_t inaccessibleDirectories = 0;
	std::chrono::milliseconds duration{0};
};

struct ScanResult
{
	std::filesystem::path root;
	std::unique_ptr<DirectoryNode> tree;
	ScanTotals totals;
	std::vector<FileRecord> largestFiles;
	std::vector<ExtensionRecord> extensions;
	std::vector<std::filesystem::path> inaccessiblePaths;
};
