#pragma once

#include <chrono>
#include <cstdint>

class ScanProgress
{
public:
	ScanProgress();

	void Update(std::uint64_t files, std::uint64_t bytes);
	void Finish(std::uint64_t files, std::uint64_t bytes) const;

private:
	[[nodiscard]] bool IsTimeToPrint();

	std::chrono::steady_clock::time_point m_lastPrint;
	std::uint32_t m_callsSinceCheck;
};
