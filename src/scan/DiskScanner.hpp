#pragma once

#include "DirectoryNode.hpp"
#include "ScanResult.hpp"
#include "scan/collectors/ExtensionStatistics.hpp"
#include "scan/collectors/TopFilesCollector.hpp"
#include "scan/enumerator/DirectoryEntry.hpp"
#include "scan/progress/ScanProgress.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class DiskScanner
{
public:
	explicit DiskScanner(std::filesystem::path root, bool showProgress = true);

	[[nodiscard]] ScanResult Scan();

private:
	struct PendingDirectory
	{
		DirectoryNode* node;
		std::filesystem::path path;
	};

	void ScanDirectory(const PendingDirectory& pending);
	void ProcessEntry(const PendingDirectory& pending, const DirectoryEntry& entry);
	void ProcessFile(const PendingDirectory& pending, const DirectoryEntry& entry);
	void ProcessSubdirectory(const PendingDirectory& pending, const DirectoryEntry& entry);
	void MarkInaccessible(const PendingDirectory& pending);
	void ReportProgress();

	std::filesystem::path m_root;
	bool m_showProgress;
	std::unique_ptr<DirectoryNode> m_tree;
	std::vector<PendingDirectory> m_pending;
	ScanTotals m_totals;
	TopFilesCollector m_topFiles;
	ExtensionStatistics m_extensions;
	std::vector<std::filesystem::path> m_inaccessiblePaths;
	ScanProgress m_progress;
};
