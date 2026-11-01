#pragma once

#ifdef _WIN32

#include <string>
#include <windows.h>

class FindHandle final
{
public:
	explicit FindHandle(const std::wstring& pattern) noexcept;
	~FindHandle() noexcept;

	FindHandle(const FindHandle&) = delete;
	FindHandle& operator=(const FindHandle&) = delete;
	FindHandle(FindHandle&&) = delete;
	FindHandle& operator=(FindHandle&&) = delete;

	[[nodiscard]] bool IsOpen() const noexcept;
	[[nodiscard]] DWORD GetOpenError() const noexcept;
	[[nodiscard]] const WIN32_FIND_DATAW& Current() const noexcept;

	bool MoveNext() noexcept;

private:
	WIN32_FIND_DATAW m_data;
	HANDLE m_handle;
	DWORD m_openError;
};

#endif
