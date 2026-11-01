#include "src/scan/collectors/TopFilesCollector.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace testing;

namespace
{
DirectoryEntry MakeFile(const char* name, const std::uint64_t size)
{
	DirectoryEntry entry;
	entry.name = name;
	entry.size = size;

	return entry;
}
} // namespace

// Проверка сохранения только самых крупных файлов
TEST(TopFilesCollectorTest, KeepsLargest)
{
	TopFilesCollector collector(2);

	collector.Offer("dir", MakeFile("a", 10));
	collector.Offer("dir", MakeFile("b", 30));
	collector.Offer("dir", MakeFile("c", 20));
	const std::vector<FileRecord> records = collector.TakeSorted();

	ASSERT_EQ(records.size(), 2u);
	EXPECT_EQ(records[0].size, 30u);
	EXPECT_EQ(records[1].size, 20u);
}

// Проверка сборки полного пути файла
TEST(TopFilesCollectorTest, BuildsPath)
{
	TopFilesCollector collector(1);

	collector.Offer("dir", MakeFile("a.bin", 5));
	const std::vector<FileRecord> records = collector.TakeSorted();

	EXPECT_EQ(records.front().path, std::filesystem::path("dir") / "a.bin");
}

// Проверка запрета нулевой ёмкости
TEST(TopFilesCollectorTest, RejectsZeroCapacity)
{
	EXPECT_THROW(TopFilesCollector(0), std::invalid_argument);
}
