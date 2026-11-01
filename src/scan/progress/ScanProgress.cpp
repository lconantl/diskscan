#include "ScanProgress.hpp"
#include "utils/format/ValueFormat.hpp"
#include <format>
#include <iostream>
#include <string>

namespace
{
constexpr std::uint32_t CALLS_BETWEEN_CLOCK_CHECKS = 64;
constexpr auto PRINT_INTERVAL = std::chrono::milliseconds(300);
constexpr std::size_t LINE_PADDING = 70;

std::string BuildProgressLine(const std::uint64_t files, const std::uint64_t bytes)
{
	std::string line
		= std::format("  Просканировано: {} файлов, {}", FormatCount(files), FormatBytes(bytes));
	if (line.size() < LINE_PADDING)
	{
		line.append(LINE_PADDING - line.size(), ' ');
	}

	return line;
}

void PrintInPlace(const std::string& line)
{
	std::cout << '\r' << line << std::flush;
}
} // namespace

ScanProgress::ScanProgress()
	: m_lastPrint(std::chrono::steady_clock::now())
	, m_callsSinceCheck(0)
{
}

void ScanProgress::Update(const std::uint64_t files, const std::uint64_t bytes)
{
	if (IsTimeToPrint())
	{
		PrintInPlace(BuildProgressLine(files, bytes));
	}
}

void ScanProgress::Finish(const std::uint64_t files, const std::uint64_t bytes) const
{
	PrintInPlace(BuildProgressLine(files, bytes));
	std::cout << std::endl;
}

bool ScanProgress::IsTimeToPrint()
{
	if (++m_callsSinceCheck < CALLS_BETWEEN_CLOCK_CHECKS)
	{
		return false;
	}
	m_callsSinceCheck = 0;

	const auto now = std::chrono::steady_clock::now();
	if (now - m_lastPrint < PRINT_INTERVAL)
	{
		return false;
	}
	m_lastPrint = now;

	return true;
}
