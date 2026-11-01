#pragma once

#include <filesystem>
#include <string>

char ParseDriveLetter(const std::string& input);

std::filesystem::path BuildDriveRoot(char letter);

std::filesystem::path ResolveDriveRoot(char letter);
