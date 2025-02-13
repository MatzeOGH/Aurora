#include "asserts.h"
#include <stdio.h>

#ifdef AURORA_PLATFORM_WINDOWS
// forward declare OutputDebugStringA and IsDebuggerPresent
extern "C" {
	typedef void* HANDLE;
	typedef void* HWND;
	typedef const char* LPCSTR;
	void __stdcall OutputDebugStringA(LPCSTR lpOutputString);
	bool __stdcall IsDebuggerPresent(void);
	bool __stdcall TerminateProcess(HANDLE hProcess, unsigned int uExitCode);
	HANDLE __stdcall GetCurrentProcess();
	int __stdcall MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, unsigned int uType);

#define MB_OK 0x00000000L
#define MB_ICONERROR 0x00000010L

}
#endif // AURORA_PLATFORM_WINDOWS



void aurora::core::reportAssert(const char* condition, const char* file, int line)
{
	char msg[4092];
	sprintf_s(msg, 4092, "%s(%d): Assertion failed:\n %s\n", file, line, condition);

	if (::IsDebuggerPresent())
	{
		::OutputDebugStringA(msg);
	}
	else
	{
		::MessageBoxA(nullptr, msg, "Assertion Failed", MB_OK | MB_ICONERROR);
		::TerminateProcess(GetCurrentProcess(), 0);
	}

}