#include "src/scan/collectors/ExtensionStatistics.hpp"
#include <gtest/gtest.h>

using namespace testing;

// Проверка объединения расширений без учёта регистра
TEST(ExtensionStatisticsTest, IgnoresCase)
{
	ExtensionStatistics statistics;

	statistics.Add("movie.MKV", 100);
	statistics.Add("clip.mkv", 50);
	const std::vector<ExtensionRecord> top = statistics.BuildTop(10);

	ASSERT_EQ(top.size(), 1u);
	EXPECT_EQ(top.front().bytes, 150u);
	EXPECT_EQ(top.front().files, 2u);
}

// Проверка подписи файлов без расширения
TEST(ExtensionStatisticsTest, NoExtension)
{
	ExtensionStatistics statistics;

	statistics.Add("Makefile", 1);

	EXPECT_EQ(statistics.BuildTop(1).front().extension, "(без расширения)");
}

// Проверка ограничения размера топа
TEST(ExtensionStatisticsTest, LimitsTop)
{
	ExtensionStatistics statistics;

	statistics.Add("a.iso", 300);
	statistics.Add("b.zip", 200);
	statistics.Add("c.txt", 100);
	const std::vector<ExtensionRecord> top = statistics.BuildTop(2);

	ASSERT_EQ(top.size(), 2u);
	EXPECT_EQ(top.front().extension, ".iso");
}
