#include "platform.h"

#include "core/asserts.h"
#include "core/logger.h"

#include <cstdio>
#include <Windows.h>



DWORD mainThreadId;
HWND serviceWindow;

static LRESULT __stdcall serviceWndProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
static DWORD WINAPI mainThread(LPVOID Param);

#define CREATE_AURORA_WINDOW (WM_USER + 0x1337)
#define DESTROY_AURORA_WINDOW (WM_USER + 0x1338)
struct Win64WindowCreateInfo {
    DWORD dwExStyle;
    LPCWSTR lpClassName;
    LPCWSTR lpWindowName;
    DWORD dwStyle;
    int X;
    int Y;
    int nWidth;
    int nHeight;
    HWND hWndParent;
    HMENU hMenu;
    HINSTANCE hInstance;
    LPVOID lpParam;
};

namespace Aurora::platform
{
	void init(const PlatformCreateInfo& platformCreateInfo)
	{
#ifdef AURORA_PLATFORM_WINDOWS
		if (true == platformCreateInfo.enableConsole) {
            AllocConsole();
            if (freopen("CONOUT$", "w", stdout) == nullptr) {
                // Handle error
            }
            if (freopen("CONOUT$", "w", stderr) == nullptr) {
                // Handle error
            }
            if (freopen("CONIN$", "r", stdin) == nullptr) {
                // Handle error
            }
		}
#else
		static_assert(false, "init is only supported on Windows");
#endif
	}


    // main thread loop
    void update()
    {
        // iterate over events send by service window
        MSG msg{};
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            switch (msg.message) {
            case WM_CLOSE: {
                SendMessageW(serviceWindow, DESTROY_AURORA_WINDOW, msg.wParam, 0);
            } break;
            }
        }

        //ImGui_ImplWin32_NewFrame();
        //ImGui::NewFrame();  // new frame

    }

    // launch a process
	void launchProcess(String path)
	{
#ifdef AURORA_PLATFORM_WINDOWS
		::ShellExecuteA(nullptr, "open", path.data, nullptr, nullptr, SW_SHOWNORMAL);
#else
		static_assert(false, "launchProcess is only supported on Windows");
#endif
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXW wc = {
        .cbSize = sizeof(wc),
        .lpfnWndProc = serviceWndProc,
        .hInstance = GetModuleHandleW(nullptr),
        .hIcon = LoadIconA(NULL, IDI_APPLICATION),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
        .lpszClassName = L"AuroraEngine",
    };

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR);
    }

    serviceWindow = CreateWindowExW(0, wc.lpszClassName, L"Service", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance, 0);

    CreateThread(0, 0, mainThread, serviceWindow, 0, &mainThreadId);

    //TODO: Add rate limit to update loop
    // Ref: https://croakingkero.com/tutorials/multi_threaded_framework/

    // this thread can just idle for the rest of the run.
    for (;;) {
        MSG msg{};
        GetMessageW(&msg, 0, 0, 0);
        TranslateMessage(&msg);
        if (  //(msg.message == WM_CHAR) || (msg.message == WM_KEYDOWN) ||
            (msg.message == WM_QUIT) || (msg.message == WM_SIZE)) {
            PostThreadMessageW(mainThreadId, msg.message, msg.wParam, msg.lParam);
        }
        else if(msg.message == WM_CHAR) {
            LOG("WM_CHAR main");
        }
        else {
            DispatchMessageW(&msg);
        }

        //Sleep(10);
    }

}

extern void AuroraMain();

// this is the main routine of a Win64 application
DWORD WINAPI mainThread(LPVOID Param) {
    serviceWindow = (HWND)Param;

    AuroraMain();

    ExitProcess(0);
}

LRESULT CALLBACK serviceWndProc(HWND Window, UINT Message, WPARAM WParam,
                                       LPARAM LParam) {
    /* NOTE(casey): This is not really a window handler per se, it's actually just
       a remote thread call handler. Windows only really has blocking remote thread
       calls if you register a WndProc for them, so that's what we do.

       This handles CREATE_DANGEROUS_WINDOW and DESTROY_DANGEROUS_WINDOW, which are
       just calls that do CreateWindow and DestroyWindow here on this thread when
       some other thread wants that to happen.
    */

    LRESULT Result = 0;

    switch (Message) {
    case CREATE_AURORA_WINDOW: {
        Win64WindowCreateInfo* createInfo = (Win64WindowCreateInfo*)WParam;
        Result = (LRESULT)CreateWindowExW(
            createInfo->dwExStyle,
            createInfo->lpClassName,
            createInfo->lpWindowName,
            createInfo->dwStyle,
            createInfo->X,
            createInfo->Y,
            createInfo->nWidth,
            createInfo->nHeight,
            createInfo->hWndParent,
            createInfo->hMenu,
            createInfo->hInstance,
            createInfo->lpParam);

        // Get the screen size to adjust the window
        HMONITOR hmon = MonitorFromWindow((HWND)Result, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hmon, &mi);

        // set fullscreen window
        // This removes the WS_OVERLAPPEDWINDOW style from the window's style, effectively converting it from a regular window to a fullscreen window
        //SetWindowLong((HWND)Result, GWL_STYLE, GetWindowLong((HWND)Result, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
        //ShowWindow((HWND)Result, SW_MAXIMIZE);

        //SetWindowLongPtr((HWND)Result, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        //SetWindowPos((HWND)Result, HWND_TOP, 0, 0, createInfo->nWidth, createInfo->nHeight, SWP_FRAMECHANGED);
        //ShowWindow((HWND)Result, SW_MAXIMIZE);
    } break;

    case DESTROY_AURORA_WINDOW: {
        if (DestroyWindow((HWND)WParam) == 0) {
            LOG("failed to close window");
        }
    } break;
    default: {
        Result = DefWindowProcW(Window, Message, WParam, LParam);
    } break;
    }

    return Result;
}
