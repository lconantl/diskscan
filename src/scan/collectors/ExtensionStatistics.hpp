#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

struct ExtensionRecord
{
	std::string extension;
	std::uint64_t bytes = 0;
	std::uint64_t files = 0;
};

class ExtensionStatistics
{
public:
	void Add(const std::filesystem::path& fileName, std::uint64_t size);

	[[nodiscard]] std::vector<ExtensionRecord> BuildTop(std::size_t limit) const;

private:
	struct Totals
	{
		std::uint64_t bytes = 0;
		std::uint64_t files = 0;
	};

	std::unordered_map<std::filesystem::path::string_type, Totals> m_totals;
};
