#pragma once

#include <chrono>
#include <cstdint>
#include <string>

std::string FormatBytes(std::uint64_t bytes);

std::string FormatPercent(std::uint64_t part, std::uint64_t total);

std::string FormatCount(std::uint64_t count);

std::string FormatUnixDate(std::int64_t unixSeconds);

std::string FormatDuration(std::chrono::milliseconds duration);

std::string FormatCurrentDateTime();
