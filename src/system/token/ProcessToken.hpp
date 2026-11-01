#pragma once

#ifdef _WIN32

#include <windows.h>

class ProcessToken final
{
public:
	ProcessToken() noexcept;
	~ProcessToken() noexcept;

	ProcessToken(const ProcessToken&) = delete;
	ProcessToken& operator=(const ProcessToken&) = delete;
	ProcessToken(ProcessToken&&) = delete;
	ProcessToken& operator=(ProcessToken&&) = delete;

	[[nodiscard]] bool IsOpen() const noexcept;
	[[nodiscard]] bool IsElevated() const noexcept;

private:
	HANDLE m_handle;
};

#endif
