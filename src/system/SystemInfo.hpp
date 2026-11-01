#pragma once

#include <cstdint>
#include <string>

struct SystemInfo
{
	std::string osName;
	std::string osVersion;
	std::string osBuild;
	std::string systemDrive;
	std::uint64_t memoryBytes = 0;
	bool isElevated = false;
};

SystemInfo ReadSystemInfo();
