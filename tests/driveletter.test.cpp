#include "src/drive/DriveLetter.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace testing;

// Проверка разбора строчной латинской буквы
TEST(DriveLetterTest, LowerLatin)
{
	EXPECT_EQ(ParseDriveLetter("c"), 'C');
}

// Проверка разбора буквы с двоеточием и слешем
TEST(DriveLetterTest, WithSeparators)
{
	EXPECT_EQ(ParseDriveLetter(" D:\\ "), 'D');
}

// Проверка буквы, набранной в русской раскладке
TEST(DriveLetterTest, CyrillicLayout)
{
	EXPECT_EQ(ParseDriveLetter("с"), 'C');
}

// Проверка отказа на пустом вводе
TEST(DriveLetterTest, RejectsEmpty)
{
	EXPECT_THROW(ParseDriveLetter("  "), std::invalid_argument);
}

// Проверка отказа на нескольких символах
TEST(DriveLetterTest, RejectsWord)
{
	EXPECT_THROW(ParseDriveLetter("CD"), std::invalid_argument);
}

// Проверка построения корня диска
TEST(DriveLetterTest, BuildsRoot)
{
	EXPECT_EQ(BuildDriveRoot('E').string(), "E:\\");
}
