#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

struct DirectoryNode
{
	std::filesystem::path name;
	std::uint64_t ownBytes = 0;
	std::uint64_t totalBytes = 0;
	std::uint64_t ownFiles = 0;
	std::uint64_t totalFiles = 0;
	bool isAccessible = true;
	std::vector<std::unique_ptr<DirectoryNode>> children;
};
