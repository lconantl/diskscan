#include "src/scan/DiskScanner.hpp"
#include "src/cleanup/CleanupAnalyzer.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

using namespace testing;

namespace
{
class TemporaryTree final
{
public:
	TemporaryTree()
		: m_root(std::filesystem::temp_directory_path() / "diskscan-test-tree")
	{
		std::filesystem::remove_all(m_root);
		WriteFile("Users/alice/AppData/Local/Temp/a.tmp", 300);
		WriteFile("Users/alice/Downloads/b.iso", 500);
		WriteFile("Windows/Temp/c.log", 200);
	}

	~TemporaryTree()
	{
		std::error_code error;
		std::filesystem::remove_all(m_root, error);
	}

	TemporaryTree(const TemporaryTree&) = delete;
	TemporaryTree& operator=(const TemporaryTree&) = delete;

	[[nodiscard]] const std::filesystem::path& Root() const
	{
		return m_root;
	}

private:
	void WriteFile(const std::string& relative, const std::size_t size) const
	{
		const std::filesystem::path path = m_root / relative;
		std::filesystem::create_directories(path.parent_path());
		std::ofstream(path, std::ios::binary) << std::string(size, 'x');
	}

	std::filesystem::path m_root;
};
} // namespace

// Проверка подсчёта общего размера и числа файлов
TEST(DiskScannerTest, CountsTotals)
{
	const TemporaryTree tree;

	const ScanResult result = DiskScanner(tree.Root(), false).Scan();

	EXPECT_EQ(result.totals.bytes, 1000u);
	EXPECT_EQ(result.totals.files, 3u);
	EXPECT_EQ(result.tree->totalBytes, 1000u);
}

// Проверка сортировки крупнейших файлов
TEST(DiskScannerTest, LargestFirst)
{
	const TemporaryTree tree;

	const ScanResult result = DiskScanner(tree.Root(), false).Scan();

	ASSERT_FALSE(result.largestFiles.empty());
	EXPECT_EQ(result.largestFiles.front().path.filename(), "b.iso");
}

// Проверка поиска временных файлов пользователей и системы
TEST(DiskScannerTest, FindsCleanupTargets)
{
	const TemporaryTree tree;
	const ScanResult result = DiskScanner(tree.Root(), false).Scan();

	const std::vector<CleanupFinding> findings = AnalyzeCleanup(*result.tree, tree.Root());

	EXPECT_EQ(findings.size(), 3u);
	EXPECT_EQ(findings.front().bytes, 500u);
}

// Проверка отказа сканировать несуществующую папку
TEST(DiskScannerTest, RejectsMissingRoot)
{
	EXPECT_THROW(DiskScanner("/definitely/missing/path", false), std::runtime_error);
}
