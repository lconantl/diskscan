#ifdef _WIN32

#include "FindHandle.hpp"

FindHandle::FindHandle(const std::wstring& pattern) noexcept
	: m_data{}
	, m_handle(FindFirstFileExW(pattern.c_str(),
		  FindExInfoBasic,
		  &m_data,
		  FindExSearchNameMatch,
		  nullptr,
		  FIND_FIRST_EX_LARGE_FETCH))
	, m_openError(m_handle == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS)
{
}

FindHandle::~FindHandle() noexcept
{
	if (IsOpen())
	{
		FindClose(m_handle);
	}
}

bool FindHandle::IsOpen() const noexcept
{
	return m_handle != INVALID_HANDLE_VALUE;
}

DWORD FindHandle::GetOpenError() const noexcept
{
	return m_openError;
}

const WIN32_FIND_DATAW& FindHandle::Current() const noexcept
{
	return m_data;
}

bool FindHandle::MoveNext() noexcept
{
	return FindNextFileW(m_handle, &m_data) != 0;
}

#endif
