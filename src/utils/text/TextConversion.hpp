#pragma once

#include <filesystem>
#include <string>

std::string ToUtf8(const std::filesystem::path& path);

std::filesystem::path FromUtf8(const std::string& text);

std::filesystem::path::string_type ToComparisonKey(const std::filesystem::path& path);

std::string Trim(const std::string& text);
