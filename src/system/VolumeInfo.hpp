#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

struct VolumeInfo
{
	std::string label;
	std::string fileSystem;
	std::string driveType;
	std::uint64_t totalBytes = 0;
	std::uint64_t freeBytes = 0;
};

VolumeInfo ReadVolumeInfo(const std::filesystem::path& root);

std::uint64_t GetUsedBytes(const VolumeInfo& volume) noexcept;
