#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

struct DirectoryEntry
{
	std::filesystem::path name;
	std::uint64_t size = 0;
	std::int64_t lastWriteUnixSeconds = 0;
	bool isDirectory = false;
	bool isLink = false;
	bool isCloudOnly = false;
};

struct DirectoryListing
{
	bool isAccessible = true;
	std::vector<DirectoryEntry> entries;
};
