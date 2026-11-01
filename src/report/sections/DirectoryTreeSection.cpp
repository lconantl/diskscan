#include "DirectoryTreeSection.hpp"
#include "utils/format/ValueFormat.hpp"
#include "utils/text/TextConversion.hpp"
#include <algorithm>
#include <cstddef>
#include <format>
#include <string>
#include <vector>

namespace
{
constexpr std::uint64_t MIN_THRESHOLD_BYTES = 512ull * 1024 * 1024;
constexpr std::uint64_t HIDDEN_MIN_BYTES = 1024ull * 1024;
constexpr std::uint64_t THRESHOLD_DIVISOR = 200;
constexpr std::size_t MAX_DEPTH = 12;
constexpr std::size_t MAX_CHILDREN = 12;

struct TreeStyle
{
	std::uint64_t thresholdBytes;
	std::uint64_t rootBytes;
};

struct TreeItem
{
	std::string text;
	const DirectoryNode* node = nullptr;
};

std::uint64_t CalculateThreshold(const std::uint64_t rootBytes)
{
	return std::max(MIN_THRESHOLD_BYTES, rootBytes / THRESHOLD_DIVISOR);
}

std::string DescribeNode(const DirectoryNode& node, const TreeStyle& style)
{
	std::string line = std::format("{}  {}  ({})",
		ToUtf8(node.name),
		FormatBytes(node.totalBytes),
		FormatPercent(node.totalBytes, style.rootBytes));

	if (!node.isAccessible)
	{
		line += "  [нет доступа]";
	}

	return line;
}

std::string DescribeOwnFiles(const DirectoryNode& node)
{
	return std::format("[файлы в самой папке]  {}  ({} шт.)",
		FormatBytes(node.ownBytes),
		FormatCount(node.ownFiles));
}

void AppendOwnFilesItem(
	std::vector<TreeItem>& items, const DirectoryNode& node, const TreeStyle& style)
{
	if (node.ownBytes >= style.thresholdBytes && !node.children.empty())
	{
		items.push_back({DescribeOwnFiles(node)});
	}
}

std::size_t AppendChildItems(
	std::vector<TreeItem>& items, const DirectoryNode& node, const TreeStyle& style)
{
	std::size_t visibleCount = 0;
	for (const auto& child : node.children)
	{
		if (child->totalBytes < style.thresholdBytes || visibleCount >= MAX_CHILDREN)
		{
			break;
		}
		items.push_back({DescribeNode(*child, style), child.get()});
		++visibleCount;
	}

	return visibleCount;
}

void AppendHiddenItem(
	std::vector<TreeItem>& items, const DirectoryNode& node, const std::size_t visibleCount)
{
	std::uint64_t hiddenBytes = 0;
	for (std::size_t index = visibleCount; index < node.children.size(); ++index)
	{
		hiddenBytes += node.children[index]->totalBytes;
	}

	const std::size_t hiddenCount = node.children.size() - visibleCount;
	if (hiddenCount > 0 && hiddenBytes >= HIDDEN_MIN_BYTES)
	{
		items.push_back(
			{std::format("… ещё папок: {}, {}", hiddenCount, FormatBytes(hiddenBytes))});
	}
}

std::vector<TreeItem> CollectItems(const DirectoryNode& node, const TreeStyle& style)
{
	std::vector<TreeItem> items;

	AppendOwnFilesItem(items, node, style);
	const std::size_t visibleCount = AppendChildItems(items, node, style);
	AppendHiddenItem(items, node, visibleCount);

	return items;
}

std::string BuildBranch(const bool isLast)
{
	return isLast ? "└── " : "├── ";
}

std::string BuildIndent(const bool isLast)
{
	return isLast ? "    " : "│   ";
}

void RenderChildren(std::string& output,
	const DirectoryNode& node,
	const std::string& prefix,
	const std::size_t depth,
	const TreeStyle& style)
{
	if (depth >= MAX_DEPTH)
	{
		return;
	}

	const std::vector<TreeItem> items = CollectItems(node, style);
	for (std::size_t index = 0; index < items.size(); ++index)
	{
		const bool isLast = index + 1 == items.size();
		output += prefix + BuildBranch(isLast) + items[index].text + "\n";

		if (items[index].node != nullptr)
		{
			RenderChildren(
				output, *items[index].node, prefix + BuildIndent(isLast), depth + 1, style);
		}
	}
}
} // namespace

void DirectoryTreeSection::Write(const ReportContext& context, MarkdownFormatter& formatter) const
{
	const DirectoryNode& root = *context.scan.tree;
	const TreeStyle style{CalculateThreshold(root.totalBytes), root.totalBytes};

	formatter.AddHeader(2, "Крупнейшие папки");
	formatter.AddText("Показаны папки от " + FormatBytes(style.thresholdBytes) + ".");

	std::string tree = DescribeNode(root, style) + "\n";
	RenderChildren(tree, root, "", 0, style);
	tree.pop_back();

	formatter.AddTree(tree);
}
