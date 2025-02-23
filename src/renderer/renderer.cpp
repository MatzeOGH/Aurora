#include "renderer.h"
#include "core/types.h"
#include "auroravulkan.h"
#include "core/arena.h"
#include "core/string.h"
#include "core/span.h"

#include "platform/window.h"

#include "renderer/Gpu.h"

// TODO: remove vk_enum_string_helper.h 
#include <vulkan/vk_enum_string_helper.h> // vk code to string

using namespace Aurora;

namespace Aurora {
	VkSurfaceKHR createSurface(VkInstance instance, struct Window window);
	VkBool32 queryQueueFamilyPresentationSupport(VkPhysicalDevice physicalDevice, u32 queueFamilyIndex);
}

VkDebugUtilsMessengerEXT debugUtilsMessengerEXT{};

VkSurfaceKHR surface{};

// vulkan device
Gpu* gpu;


void Aurora::Renderer::init(const RendererCreateInfo& rendererCreateInfo, Arena* persistane, Arena scratch)
{
	gpu = persistane->alloc<Gpu>();
	gpu->init(
		{ 
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
			VK_EXT_SHADER_OBJECT_EXTENSION_NAME 
		},
		scratch);
}

void Aurora::Renderer::registerWindow(Window window)
{
	surface = createSurface(gpu->instance, window);
}

void Aurora::Renderer::unregisterWindow(Aurora::Window window)
{
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(gpu->instance, surface, nullptr);
	}
}

void Aurora::Renderer::shutdown(Arena scratch)
{
	gpu->shutdown(scratch);
}

const char* toConstChar(VkResult result)
{
	return string_VkResult(result);
}
