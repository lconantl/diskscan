#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

enum class CleanupMethod
{
	Safe,
	SystemTool,
	Review,
	Keep,
};

enum class TargetKind
{
	Directory,
	File,
};

struct CleanupRule
{
	std::string category;
	std::filesystem::path pattern;
	CleanupMethod method;
	std::string advice;
	TargetKind kind = TargetKind::Directory;
};

struct CleanupFinding
{
	std::string category;
	std::filesystem::path path;
	std::uint64_t bytes = 0;
	std::uint64_t files = 0;
	CleanupMethod method = CleanupMethod::Review;
	std::string advice;
};

std::string DescribeMethod(CleanupMethod method);
