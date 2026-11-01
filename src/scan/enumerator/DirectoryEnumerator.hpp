#pragma once

#include "DirectoryEntry.hpp"
#include <filesystem>
#include <optional>

DirectoryListing ListDirectory(const std::filesystem::path& directory);

std::optional<DirectoryEntry> FindEntry(const std::filesystem::path& path);
