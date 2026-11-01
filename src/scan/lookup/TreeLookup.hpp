#pragma once

#include "scan/DirectoryNode.hpp"
#include <filesystem>
#include <vector>

struct TreeMatch
{
	const DirectoryNode* node = nullptr;
	std::filesystem::path relativePath;
};

std::vector<TreeMatch> FindMatches(const DirectoryNode& root, const std::filesystem::path& pattern);

const DirectoryNode* FindChild(const DirectoryNode& parent, const std::filesystem::path& name);

const DirectoryNode* FindDescendant(
	const DirectoryNode& root, const std::filesystem::path& relative);
