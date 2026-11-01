#include "src/scan/lookup/TreeLookup.hpp"
#include <gtest/gtest.h>

using namespace testing;

namespace
{
DirectoryNode& AddChild(DirectoryNode& parent, const char* name, const std::uint64_t bytes)
{
	auto child = std::make_unique<DirectoryNode>();
	child->name = name;
	child->totalBytes = bytes;

	return *parent.children.emplace_back(std::move(child));
}

std::unique_ptr<DirectoryNode> BuildUsersTree()
{
	auto root = std::make_unique<DirectoryNode>();
	DirectoryNode& users = AddChild(*root, "Users", 0);
	AddChild(AddChild(users, "alice", 0), "Temp", 10);
	AddChild(AddChild(users, "bob", 0), "Temp", 20);

	return root;
}
} // namespace

// Проверка поиска по подстановке *
TEST(TreeLookupTest, WildcardMatches)
{
	const std::unique_ptr<DirectoryNode> root = BuildUsersTree();

	const std::vector<TreeMatch> matches = FindMatches(*root, "Users/*/Temp");

	EXPECT_EQ(matches.size(), 2u);
}

// Проверка поиска без учёта регистра
TEST(TreeLookupTest, CaseInsensitive)
{
	const std::unique_ptr<DirectoryNode> root = BuildUsersTree();

	const DirectoryNode* node = FindDescendant(*root, "users/BOB/temp");

	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->totalBytes, 20u);
}

// Проверка отсутствующего пути
TEST(TreeLookupTest, MissingPath)
{
	const std::unique_ptr<DirectoryNode> root = BuildUsersTree();

	EXPECT_TRUE(FindMatches(*root, "Users/*/Downloads").empty());
}
