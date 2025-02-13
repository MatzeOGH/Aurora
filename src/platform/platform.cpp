#include "platform.h"

#ifdef AURORA_PLATFORM_WINDOWS
// forward declare Windows API
typedef int BOOL;
typedef void* HINSTANCE;
extern "C" __declspec(dllimport) BOOL __stdcall FreeConsole(void);
extern "C" __declspec(dllimport) HINSTANCE __stdcall ShellExecuteA(void* hwnd, const char* lpOperation, const char* lpFile, const char* lpParameters, const char* lpDirectory, int nShowCmd);
#endif


namespace aurora::platform
{
	void init(const PlatformCreateInfo& platformCreateInfo)
	{
#ifdef AURORA_PLATFORM_WINDOWS
		if (platformCreateInfo.enableConsole == false) {
			::FreeConsole();
		}
#else
		static_assert(false, "init is only supported on Windows");
#endif
	}

	// launch a process
	void launchProcess(const char* path)
	{
#ifdef AURORA_PLATFORM_WINDOWS
		constexpr int SW_SHOWNORMAL = 1;
		::ShellExecuteA(nullptr, "open", path, nullptr, nullptr, SW_SHOWNORMAL);
#else
		static_assert(false, "launchProcess is only supported on Windows");
#endif
	}
}
