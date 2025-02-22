#include "window.h"
#include "core/logger.h"

#include <Windows.h>

extern DWORD mainThreadId;
extern HWND serviceWindow;

#define CREATE_AURORA_WINDOW (WM_USER + 0x1337)
#define DESTROY_AURORA_WINDOW (WM_USER + 0x1338)
#define WINDOW_CLASS_NAME L"AuroraWindowClass"

static LRESULT CALLBACK DisplayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT Result = 0;

    switch (msg) {
        // NOTE(casey): Mildly annoying, if you want to specify a window, you have
        // to snuggle the params yourself, because Windows doesn't let you forward
        // a god damn window message even though the program IS CALLED WINDOWS. It's
        // in the name! Let me pass it!
    case WM_CLOSE: {
        PostThreadMessageW(mainThreadId, msg, (WPARAM)hWnd, lParam);
    } break;

    // NOTE(casey): Anything you want the application to handle, forward to the main thread
    // here.
    /*
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_DESTROY:
    */
    case WM_CHAR: {
        LOG("hello");
        //PostThreadMessageW(mainThreadId, msg, wParam, lParam);
    } break;
    default: {
        //if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        //    Result = 1;
        //else
            Result = DefWindowProcW(hWnd, msg, wParam, lParam);
    } break;
    }

    return Result;
}


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

addptr Aurora::platform::createWindow(const WindowCreateInfo& windowCreateInfo)
{
    // create window class
    WNDCLASSEXW windowClass = {
        .cbSize = sizeof(windowClass),
        .lpfnWndProc = &DisplayWndProc,
        .hInstance = GetModuleHandleW(NULL),
        .hIcon = LoadIconA(NULL, IDI_APPLICATION),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
        .lpszClassName = WINDOW_CLASS_NAME,
    };
    RegisterClassExW(&windowClass);

    Win64WindowCreateInfo win64CreateInfo{
        .dwExStyle = 0,
        .lpClassName = windowClass.lpszClassName,
        .lpWindowName = L"AURORA Window",
        .dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        .X = CW_USEDEFAULT,
        .Y = CW_USEDEFAULT,
        .nWidth = CW_USEDEFAULT,
        .nHeight = CW_USEDEFAULT,
        .hInstance = windowClass.hInstance,
    };

    HWND windowHandle = (HWND)SendMessageW(serviceWindow, CREATE_AURORA_WINDOW, (WPARAM)&win64CreateInfo, 0);

    return (addptr)windowHandle;
}

i32 Aurora::platform::getWindowCount()
{
    int windowCount = 0;
    for (HWND window = FindWindowExW(0, 0, WINDOW_CLASS_NAME, 0);
         window;
         window = FindWindowExW(0, window, WINDOW_CLASS_NAME, 0)) {
        windowCount++;
    }
    return windowCount;
}