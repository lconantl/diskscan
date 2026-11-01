#include "ExtensionStatistics.hpp"
#include "utils/text/TextConversion.hpp"
#include <algorithm>

namespace
{
constexpr auto NO_EXTENSION_LABEL = "(без расширения)";

std::string DescribeExtension(const std::filesystem::path::string_type& key)
{
	if (key.empty())
	{
		return NO_EXTENSION_LABEL;
	}

	return ToUtf8(std::filesystem::path(key));
}

bool IsHeavier(const ExtensionRecord& left, const ExtensionRecord& right)
{
	return left.bytes > right.bytes;
}
} // namespace

void ExtensionStatistics::Add(const std::filesystem::path& fileName, const std::uint64_t size)
{
	Totals& totals = m_totals[ToComparisonKey(fileName.extension())];
	totals.bytes += size;
	++totals.files;
}

std::vector<ExtensionRecord> ExtensionStatistics::BuildTop(const std::size_t limit) const
{
	std::vector<ExtensionRecord> records;
	records.reserve(m_totals.size());

	for (const auto& [key, totals] : m_totals)
	{
		records.push_back({DescribeExtension(key), totals.bytes, totals.files});
	}

	std::sort(records.begin(), records.end(), IsHeavier);
	if (records.size() > limit)
	{
		records.resize(limit);
	}

	return records;
}
