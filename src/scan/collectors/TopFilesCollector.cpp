#include "TopFilesCollector.hpp"
#include <algorithm>
#include <stdexcept>

namespace
{
void AssertIsCapacityPositive(const std::size_t capacity)
{
	if (capacity == 0)
	{
		throw std::invalid_argument("Размер списка крупнейших файлов должен быть больше нуля");
	}
}

bool IsLarger(const FileRecord& left, const FileRecord& right)
{
	return left.size > right.size;
}
} // namespace

bool FileRecordGreater::operator()(const FileRecord& left, const FileRecord& right) const noexcept
{
	return left.size > right.size;
}

TopFilesCollector::TopFilesCollector(const std::size_t capacity)
	: m_capacity(capacity)
{
	AssertIsCapacityPositive(m_capacity);
}

void TopFilesCollector::Offer(const std::filesystem::path& directory, const DirectoryEntry& entry)
{
	if (!IsWorthKeeping(entry.size))
	{
		return;
	}

	m_heap.push({directory / entry.name, entry.size, entry.lastWriteUnixSeconds});
	if (m_heap.size() > m_capacity)
	{
		m_heap.pop();
	}
}

std::vector<FileRecord> TopFilesCollector::TakeSorted()
{
	std::vector<FileRecord> records;
	records.reserve(m_heap.size());

	while (!m_heap.empty())
	{
		records.push_back(m_heap.top());
		m_heap.pop();
	}

	std::sort(records.begin(), records.end(), IsLarger);
	return records;
}

bool TopFilesCollector::IsWorthKeeping(const std::uint64_t size) const noexcept
{
	return m_heap.size() < m_capacity || size > m_heap.top().size;
}
