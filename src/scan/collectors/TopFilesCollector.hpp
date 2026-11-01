#pragma once

#include "scan/enumerator/DirectoryEntry.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <queue>
#include <vector>

struct FileRecord
{
	std::filesystem::path path;
	std::uint64_t size = 0;
	std::int64_t lastWriteUnixSeconds = 0;
};

struct FileRecordGreater
{
	bool operator()(const FileRecord& left, const FileRecord& right) const noexcept;
};

class TopFilesCollector
{
public:
	explicit TopFilesCollector(std::size_t capacity);

	void Offer(const std::filesystem::path& directory, const DirectoryEntry& entry);

	[[nodiscard]] std::vector<FileRecord> TakeSorted();

private:
	[[nodiscard]] bool IsWorthKeeping(std::uint64_t size) const noexcept;

	std::size_t m_capacity;
	std::priority_queue<FileRecord, std::vector<FileRecord>, FileRecordGreater> m_heap;
};
