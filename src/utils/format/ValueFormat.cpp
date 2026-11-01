#include "ValueFormat.hpp"
#include <array>
#include <ctime>
#include <format>

namespace
{
constexpr std::array<const char*, 5> SIZE_UNITS = {"Б", "КБ", "МБ", "ГБ", "ТБ"};
constexpr double SIZE_STEP = 1024.0;
constexpr std::int64_t SECONDS_PER_MINUTE = 60;

std::size_t SelectUnitIndex(double& value)
{
	std::size_t index = 0;
	while (value >= SIZE_STEP && index + 1 < SIZE_UNITS.size())
	{
		value /= SIZE_STEP;
		++index;
	}

	return index;
}

std::string GroupDigits(const std::string& digits)
{
	std::string result;
	const std::size_t length = digits.size();

	for (std::size_t index = 0; index < length; ++index)
	{
		if (index > 0 && (length - index) % 3 == 0)
		{
			result += ' ';
		}
		result += digits[index];
	}

	return result;
}

std::tm ToLocalTime(const std::time_t moment)
{
	std::tm local{};
#ifdef _WIN32
	localtime_s(&local, &moment);
#else
	localtime_r(&moment, &local);
#endif
	return local;
}
} // namespace

std::string FormatBytes(const std::uint64_t bytes)
{
	double value = static_cast<double>(bytes);
	const std::size_t unitIndex = SelectUnitIndex(value);

	if (unitIndex == 0)
	{
		return std::format("{} {}", bytes, SIZE_UNITS[unitIndex]);
	}

	return std::format("{:.2f} {}", value, SIZE_UNITS[unitIndex]);
}

std::string FormatPercent(const std::uint64_t part, const std::uint64_t total)
{
	if (total == 0)
	{
		return "—";
	}

	const double percent = static_cast<double>(part) * 100.0 / static_cast<double>(total);
	return std::format("{:.1f}%", percent);
}

std::string FormatCount(const std::uint64_t count)
{
	return GroupDigits(std::to_string(count));
}

std::string FormatUnixDate(const std::int64_t unixSeconds)
{
	using namespace std::chrono;

	const sys_days days = floor<std::chrono::days>(sys_seconds(seconds(unixSeconds)));
	const year_month_day date(days);

	return std::format("{:04}-{:02}-{:02}",
		static_cast<int>(date.year()),
		static_cast<unsigned>(date.month()),
		static_cast<unsigned>(date.day()));
}

std::string FormatDuration(const std::chrono::milliseconds duration)
{
	const std::int64_t totalSeconds = duration.count() / 1000;
	const std::int64_t minutes = totalSeconds / SECONDS_PER_MINUTE;
	const std::int64_t seconds = totalSeconds % SECONDS_PER_MINUTE;

	return std::format("{} мин {} сек", minutes, seconds);
}

std::string FormatCurrentDateTime()
{
	const std::tm local = ToLocalTime(std::time(nullptr));

	return std::format("{:04}-{:02}-{:02} {:02}:{:02}",
		local.tm_year + 1900,
		local.tm_mon + 1,
		local.tm_mday,
		local.tm_hour,
		local.tm_min);
}
