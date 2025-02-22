#pragma once

#include "core/logger.h"

#ifdef AURORA_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef VOLK
#include "volk.h"
#else
#include "vulkan/vulkan.h"
#endif

const char* toConstChar(VkResult result);

#define VK_CHECK(x) do{VkResult result = x; if (result != VK_SUCCESS) { LOG("Error calling:\n%s\nResult: %s\n%s, %d\n", #x, toConstChar(err), __FILE__, __LINE__); } } while(0)