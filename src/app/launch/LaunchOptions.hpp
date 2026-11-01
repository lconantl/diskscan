#pragma once

#include <optional>
#include <string>

struct LaunchOptions
{
	std::optional<std::string> driveInput;
	bool isHelpRequested = false;
};
