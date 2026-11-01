#include "src/utils/formatter/MarkdownFormatter.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace testing;

// Проверка построения таблицы
TEST(MarkdownFormatterTest, BuildsTable)
{
	MarkdownFormatter formatter;

	formatter.AddTable({"A", "B"}, {{"1", "2"}});

	EXPECT_EQ(formatter.Build(), "| A | B |\n| --- | --- |\n| 1 | 2 |\n\n");
}

// Проверка экранирования вертикальной черты
TEST(MarkdownFormatterTest, EscapesPipe)
{
	MarkdownFormatter formatter;

	formatter.AddTable({"A"}, {{"x|y"}});

	EXPECT_NE(formatter.Build().find("x\\|y"), std::string::npos);
}

// Проверка отказа на строке неверной ширины
TEST(MarkdownFormatterTest, RejectsWrongRow)
{
	MarkdownFormatter formatter;

	EXPECT_THROW(formatter.AddTable({"A", "B"}, {{"1"}}), std::invalid_argument);
}
