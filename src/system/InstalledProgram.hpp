#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct InstalledProgram
{
	std::string name;
	std::string publisher;
	std::uint64_t estimatedBytes = 0;
	std::filesystem::path location;
};

std::vector<InstalledProgram> ReadInstalledPrograms();
