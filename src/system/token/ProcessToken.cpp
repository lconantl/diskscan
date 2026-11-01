#ifdef _WIN32

#include "ProcessToken.hpp"

namespace
{
HANDLE OpenCurrentProcessToken() noexcept
{
	HANDLE handle = nullptr;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &handle) == 0)
	{
		return nullptr;
	}

	return handle;
}
} // namespace

ProcessToken::ProcessToken() noexcept
	: m_handle(OpenCurrentProcessToken())
{
}

ProcessToken::~ProcessToken() noexcept
{
	if (m_handle != nullptr)
	{
		CloseHandle(m_handle);
	}
}

bool ProcessToken::IsOpen() const noexcept
{
	return m_handle != nullptr;
}

bool ProcessToken::IsElevated() const noexcept
{
	if (!IsOpen())
	{
		return false;
	}

	TOKEN_ELEVATION elevation{};
	DWORD size = 0;
	const BOOL success
		= GetTokenInformation(m_handle, TokenElevation, &elevation, sizeof(elevation), &size);

	return success != 0 && elevation.TokenIsElevated != 0;
}

#endif
