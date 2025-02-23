#include "../auroravulkan.h"
#include "core/types.h"
#include "platform/window.h"

typedef const char* LPCSTR;
typedef struct HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;
typedef struct HWND__* HWND;

extern "C" __declspec(dllimport) HMODULE __stdcall GetModuleHandleA(LPCSTR);

namespace Aurora
{

    VkSurfaceKHR createSurface(VkInstance instance, Window window) 
    {
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = GetModuleHandleA(nullptr),
            .hwnd = reinterpret_cast<HWND>(window.handle),
        };

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
        return surface;
    }

    VkBool32 queryQueueFamilyPresentationSupport(VkPhysicalDevice physicalDevice, u32 queueFamilyIndex) {
        return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
    }

}