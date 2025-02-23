#include "Gpu.h"


#include "core/span.h"
#include "core/string.h"
#include "core/logger.h"

namespace Aurora
{

VkBool32 queryQueueFamilyPresentationSupport(VkPhysicalDevice physicalDevice, u32 queueFamilyIndex);

VkPhysicalDevice selectPhysicalDevice(VkInstance vkInstance, Arena scratch)
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
	ASSERT(physicalDeviceCount > 0 && "No Vulkan physical devices found");

	VkPhysicalDevice* physicalDevices = scratch.alloc<VkPhysicalDevice>(physicalDeviceCount);
	VkPhysicalDeviceProperties* deviceProperties = scratch.alloc<VkPhysicalDeviceProperties>(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices);

	for (u32 i = 0; i < physicalDeviceCount; i++) {
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			LOG("Using discrete GPU %s", deviceProperties.deviceName);
			return physicalDevices[i];
		}
	}

	// fallback to the first device
	LOG("No dedicated GPU found, falling back to the first device");
	return physicalDevices[0];
}

u32 getQueueFamilyIndex(VkPhysicalDevice physicalDevice, u32 flags, Arena scratch)
{
	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties* queueFamilyProperties = scratch.alloc<VkQueueFamilyProperties>();
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);

	for (u32 i = 0; i < queueFamilyCount; i++) {
		if ((queueFamilyProperties[i].queueFlags & flags) == flags)
		{
			//TODO: check if using the same function for all queues is problematic
			// if checking for graphics queue check if it support presentation too
			if (flags & VK_QUEUE_GRAPHICS_BIT) {
				// check for surface support
				VkBool32 surfaceSupported = queryQueueFamilyPresentationSupport(physicalDevice, i);
				if (surfaceSupported == VK_TRUE) {
					return i;
				}
			}

			return i;
		}
	}

	return VK_QUEUE_FAMILY_IGNORED;
}


void Gpu::init( Span<String> deviceExtensions, Arena scratch)
{
	// create Instance

	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Aurora",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName = "Aurora Engine",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_4
	};

	const char* instanceExtensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef AURORA_PLATFORM_WINDOWS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif // AURORA_PLATFORM_WINDOWS
#ifdef _DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // _DEBUG
		VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
	};

	VkInstanceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]),
		.ppEnabledExtensionNames = instanceExtensions
	};

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	ASSERT(result == VK_SUCCESS && "Failed to create VkInstance");

#ifdef VOLK
		// Load instance-level Vulkan functions
		volkLoadInstanceOnly(instance);
#endif // VOLK

	// select physical device
	{
		physicalDevice = selectPhysicalDevice(instance, scratch);
	}

	// check if the device supports the extensions
	{
		u32 extensionCount{};
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
		VkExtensionProperties* extensionProperties = scratch.alloc<VkExtensionProperties>(extensionCount);
		String* extensionNames = scratch.alloc<String>(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties);

		for (u32 i = 0; i < extensionCount; i++) {
			extensionNames[i] = String{ &scratch, extensionProperties[i].extensionName };
		}

		auto checkForExtension = [extensionNames, extensionCount](String extension) {
			for (u32 i = 0; i < extensionCount; i++) {
				if (extension == extensionNames[i]) {
					return true;
				}
			}
			return false;
			};

		for (i32 i = 0; i < deviceExtensions.size; i++) {
			if (!checkForExtension(deviceExtensions[i])) {
				LOG("Extension %s not supported", deviceExtensions[i].data);
				ASSERT(false && "Extension not supported by device");
			}
		}
	}

	// get graphics and present queue family indices
	{
		// get queue family indices
		graphicsQueueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, scratch);
		ASSERT(graphicsQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED && "No graphics queue found");
		// look for queue that only has the VK_QUEUE_GRAPHICS_BIT bit set
		presentQueueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT, scratch);
		ASSERT(presentQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED && "No present queue found");
	}

	{
		const char** extensionNamesCStr = scratch.alloc<const char*>(deviceExtensions.size);
		for (i32 i = 0; i < deviceExtensions.size; i++) {
			extensionNamesCStr[i] = deviceExtensions[i].data;
		}

		VkDeviceQueueCreateInfo* queueCreateInfos = scratch.alloc<VkDeviceQueueCreateInfo>(2);
		float queuePriorities{ 1.0f };
		queueCreateInfos[0] = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = graphicsQueueFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriorities
		};

		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
		{
			queueCreateInfos[1] = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = presentQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriorities
			};
		}

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.queueCreateInfoCount = graphicsQueueFamilyIndex != presentQueueFamilyIndex ? 2u : 1u,
			.pQueueCreateInfos = queueCreateInfos,
			.enabledExtensionCount = (u32)deviceExtensions.size,
			.ppEnabledExtensionNames = extensionNamesCStr,
		};

		result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		ASSERT(result == VK_SUCCESS);

		// get queues from the device
		vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
		{
			vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
		}
		else
		{
			presentQueue = graphicsQueue;
		}
	}

}

void Gpu::shutdown(Arena scratch)
{
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

} // namespace Aurora