#include "CleanupAnalyzer.hpp"
#include "CleanupCatalog.hpp"
#include "scan/enumerator/DirectoryEnumerator.hpp"
#include "scan/lookup/TreeLookup.hpp"
#include <algorithm>
#include <optional>

namespace
{
std::filesystem::path BuildDisplayPath(
	const std::filesystem::path& driveRoot, const std::filesystem::path& relative)
{
	std::filesystem::path path = driveRoot / relative;
	path.make_preferred();

	return path;
}

CleanupFinding CreateFinding(const CleanupRule& rule,
	std::filesystem::path path,
	const std::uint64_t bytes,
	const std::uint64_t files)
{
	return {rule.category, std::move(path), bytes, files, rule.method, rule.advice};
}

void AppendIfNotEmpty(std::vector<CleanupFinding>& findings, CleanupFinding finding)
{
	if (finding.bytes > 0)
	{
		findings.push_back(std::move(finding));
	}
}

void AnalyzeFileRule(const CleanupRule& rule,
	const std::filesystem::path& driveRoot,
	std::vector<CleanupFinding>& findings)
{
	const std::filesystem::path path = BuildDisplayPath(driveRoot, rule.pattern);
	const std::optional<DirectoryEntry> entry = FindEntry(path);

	if (entry.has_value() && !entry->isDirectory)
	{
		AppendIfNotEmpty(findings, CreateFinding(rule, path, entry->size, 1));
	}
}

void AnalyzeDirectoryRule(const CleanupRule& rule,
	const DirectoryNode& tree,
	const std::filesystem::path& driveRoot,
	std::vector<CleanupFinding>& findings)
{
	for (const auto& match : FindMatches(tree, rule.pattern))
	{
		const std::filesystem::path path = BuildDisplayPath(driveRoot, match.relativePath);
		AppendIfNotEmpty(
			findings, CreateFinding(rule, path, match.node->totalBytes, match.node->totalFiles));
	}
}

void AnalyzeRule(const CleanupRule& rule,
	const DirectoryNode& tree,
	const std::filesystem::path& driveRoot,
	std::vector<CleanupFinding>& findings)
{
	if (rule.kind == TargetKind::File)
	{
		AnalyzeFileRule(rule, driveRoot, findings);
		return;
	}

	AnalyzeDirectoryRule(rule, tree, driveRoot, findings);
}

bool IsLargerFinding(const CleanupFinding& left, const CleanupFinding& right)
{
	return left.bytes > right.bytes;
}
} // namespace

std::vector<CleanupFinding> AnalyzeCleanup(
	const DirectoryNode& tree, const std::filesystem::path& driveRoot)
{
	std::vector<CleanupFinding> findings;
	for (const auto& rule : BuildCleanupCatalog())
	{
		AnalyzeRule(rule, tree, driveRoot, findings);
	}

	std::stable_sort(findings.begin(), findings.end(), IsLargerFinding);
	return findings;
}
