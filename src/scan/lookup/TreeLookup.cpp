#include "TreeLookup.hpp"
#include "utils/text/TextConversion.hpp"

namespace
{
constexpr auto WILDCARD = "*";

std::vector<std::filesystem::path> SplitComponents(const std::filesystem::path& path)
{
	std::vector<std::filesystem::path> components;
	for (const auto& component : path.relative_path())
	{
		if (!component.empty())
		{
			components.push_back(component);
		}
	}

	return components;
}

bool IsWildcard(const std::filesystem::path& component)
{
	return component == WILDCARD;
}

bool IsSameName(const std::filesystem::path& left, const std::filesystem::path& right)
{
	return ToComparisonKey(left) == ToComparisonKey(right);
}

std::vector<TreeMatch> ExpandWildcard(const std::vector<TreeMatch>& current)
{
	std::vector<TreeMatch> next;
	for (const auto& match : current)
	{
		for (const auto& child : match.node->children)
		{
			next.push_back({child.get(), match.relativePath / child->name});
		}
	}

	return next;
}

std::vector<TreeMatch> ExpandExact(
	const std::vector<TreeMatch>& current, const std::filesystem::path& component)
{
	std::vector<TreeMatch> next;
	for (const auto& match : current)
	{
		if (const DirectoryNode* child = FindChild(*match.node, component))
		{
			next.push_back({child, match.relativePath / child->name});
		}
	}

	return next;
}

std::vector<TreeMatch> ExpandComponent(
	const std::vector<TreeMatch>& current, const std::filesystem::path& component)
{
	return IsWildcard(component) ? ExpandWildcard(current) : ExpandExact(current, component);
}
} // namespace

std::vector<TreeMatch> FindMatches(const DirectoryNode& root, const std::filesystem::path& pattern)
{
	std::vector<TreeMatch> matches = {{&root, {}}};

	for (const auto& component : SplitComponents(pattern))
	{
		matches = ExpandComponent(matches, component);
		if (matches.empty())
		{
			break;
		}
	}

	return matches;
}

const DirectoryNode* FindChild(const DirectoryNode& parent, const std::filesystem::path& name)
{
	for (const auto& child : parent.children)
	{
		if (IsSameName(child->name, name))
		{
			return child.get();
		}
	}

	return nullptr;
}

const DirectoryNode* FindDescendant(
	const DirectoryNode& root, const std::filesystem::path& relative)
{
	const DirectoryNode* current = &root;
	for (const auto& component : SplitComponents(relative))
	{
		current = FindChild(*current, component);
		if (current == nullptr)
		{
			return nullptr;
		}
	}

	return current;
}
