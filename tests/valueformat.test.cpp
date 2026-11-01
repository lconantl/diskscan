#include "src/utils/format/ValueFormat.hpp"
#include <gtest/gtest.h>

using namespace testing;

// Проверка вывода байтов без дробной части
TEST(ValueFormatTest, PlainBytes)
{
	EXPECT_EQ(FormatBytes(512), "512 Б");
}

// Проверка перевода в гигабайты
TEST(ValueFormatTest, Gigabytes)
{
	EXPECT_EQ(FormatBytes(3ull * 1024 * 1024 * 1024), "3.00 ГБ");
}

// Проверка защиты от деления на ноль в процентах
TEST(ValueFormatTest, PercentOfZero)
{
	EXPECT_EQ(FormatPercent(10, 0), "—");
}

// Проверка расчёта процента
TEST(ValueFormatTest, Percent)
{
	EXPECT_EQ(FormatPercent(1, 4), "25.0%");
}

// Проверка группировки разрядов
TEST(ValueFormatTest, GroupedCount)
{
	EXPECT_EQ(FormatCount(1234567), "1 234 567");
}

// Проверка форматирования даты из unix-времени
TEST(ValueFormatTest, UnixDate)
{
	EXPECT_EQ(FormatUnixDate(86400), "1970-01-02");
}
