#include "DiskScanner.hpp"
#include "scan/enumerator/DirectoryEnumerator.hpp"
#include "utils/text/TextConversion.hpp"
#include <algorithm>
#include <stdexcept>
#include <system_error>

namespace
{
constexpr std::size_t TOP_FILES_CAPACITY = 60;
constexpr std::size_t TOP_EXTENSIONS_LIMIT = 30;
constexpr std::size_t INACCESSIBLE_PATHS_LIMIT = 40;

void AssertIsRootDirectory(const std::filesystem::path& root)
{
	std::error_code error;
	if (!std::filesystem::is_directory(root, error))
	{
		throw std::runtime_error("Корень сканирования не является папкой: " + ToUtf8(root));
	}
}

std::unique_ptr<DirectoryNode> CreateNode(const std::filesystem::path& name)
{
	auto node = std::make_unique<DirectoryNode>();
	node->name = name;

	return node;
}

bool IsLargerNode(
	const std::unique_ptr<DirectoryNode>& left, const std::unique_ptr<DirectoryNode>& right)
{
	return left->totalBytes > right->totalBytes;
}

void AccumulateSizes(DirectoryNode& node)
{
	node.totalBytes = node.ownBytes;
	node.totalFiles = node.ownFiles;

	for (const auto& child : node.children)
	{
		AccumulateSizes(*child);
		node.totalBytes += child->totalBytes;
		node.totalFiles += child->totalFiles;
	}

	std::sort(node.children.begin(), node.children.end(), IsLargerNode);
}

std::chrono::milliseconds MeasureSince(const std::chrono::steady_clock::time_point start)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - start);
}
} // namespace

DiskScanner::DiskScanner(std::filesystem::path root, const bool showProgress)
	: m_root(std::move(root))
	, m_showProgress(showProgress)
	, m_tree(CreateNode(m_root))
	, m_topFiles(TOP_FILES_CAPACITY)
{
	AssertIsRootDirectory(m_root);
}

ScanResult DiskScanner::Scan()
{
	const auto start = std::chrono::steady_clock::now();

	m_pending.push_back({m_tree.get(), m_root});
	while (!m_pending.empty())
	{
		const PendingDirectory pending = std::move(m_pending.back());
		m_pending.pop_back();
		ScanDirectory(pending);
	}

	AccumulateSizes(*m_tree);
	m_totals.duration = MeasureSince(start);

	if (m_showProgress)
	{
		m_progress.Finish(m_totals.files, m_totals.bytes);
	}

	return {m_root,
		std::move(m_tree),
		m_totals,
		m_topFiles.TakeSorted(),
		m_extensions.BuildTop(TOP_EXTENSIONS_LIMIT),
		std::move(m_inaccessiblePaths)};
}

void DiskScanner::ScanDirectory(const PendingDirectory& pending)
{
	const DirectoryListing listing = ListDirectory(pending.path);
	if (!listing.isAccessible)
	{
		MarkInaccessible(pending);
		return;
	}

	for (const auto& entry : listing.entries)
	{
		ProcessEntry(pending, entry);
	}

	ReportProgress();
}

void DiskScanner::ProcessEntry(const PendingDirectory& pending, const DirectoryEntry& entry)
{
	if (entry.isLink)
	{
		++m_totals.skippedLinks;
		return;
	}

	if (entry.isDirectory)
	{
		ProcessSubdirectory(pending, entry);
		return;
	}

	ProcessFile(pending, entry);
}

void DiskScanner::ProcessFile(const PendingDirectory& pending, const DirectoryEntry& entry)
{
	if (entry.isCloudOnly)
	{
		++m_totals.cloudOnlyFiles;
		m_totals.cloudOnlyBytes += entry.size;
		return;
	}

	++m_totals.files;
	m_totals.bytes += entry.size;
	++pending.node->ownFiles;
	pending.node->ownBytes += entry.size;

	m_topFiles.Offer(pending.path, entry);
	m_extensions.Add(entry.name, entry.size);
}

void DiskScanner::ProcessSubdirectory(const PendingDirectory& pending, const DirectoryEntry& entry)
{
	++m_totals.directories;

	std::unique_ptr<DirectoryNode>& child
		= pending.node->children.emplace_back(CreateNode(entry.name));
	m_pending.push_back({child.get(), pending.path / entry.name});
}

void DiskScanner::MarkInaccessible(const PendingDirectory& pending)
{
	pending.node->isAccessible = false;
	++m_totals.inaccessibleDirectories;

	if (m_inaccessiblePaths.size() < INACCESSIBLE_PATHS_LIMIT)
	{
		m_inaccessiblePaths.push_back(pending.path);
	}
}

void DiskScanner::ReportProgress()
{
	if (m_showProgress)
	{
		m_progress.Update(m_totals.files, m_totals.bytes);
	}
}
