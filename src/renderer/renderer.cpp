#include "renderer.h"
#include "core/types.h"
#include "auroravulkan.h"
#include "core/arena.h"
#include "core/string.h"
#include "core/span.h"

// TODO: remove vk_enum_string_helper.h 
#include <vulkan/vk_enum_string_helper.h> // vk code to string

using namespace Aurora;

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

VkInstance getVkInstance()
{
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

	VkInstance instance{ VK_NULL_HANDLE };
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS) {
		LOG("Failed to create Vulkan instance");
	}
	else {
		LOG("Vulkan instance created");
	}

#ifdef VOLK
	// Load instance-level Vulkan functions
	volkLoadInstanceOnly(instance);
#endif // VOLK

	return instance;
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
			return i;
		}
	}

	return VK_QUEUE_FAMILY_IGNORED;
}

VkDevice createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, Span<String> extensions, Arena scratch)
{
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

		for (i32 i = 0; i < extensions.size; i++) {
			if (!checkForExtension(extensions[i])) {
				LOG("Extension %s not supported", extensions[i].data);
				return VK_NULL_HANDLE;
			}
		}
	}
	
	// get graphics and present queue family indices

	// get queue family indices
	u32 graphicsQueueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, scratch);
	ASSERT(graphicsQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED && "No graphics queue found");
	// look for queue that only has the VK_QUEUE_GRAPHICS_BIT bit set
	u32 presentQueueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT, scratch);
	ASSERT(presentQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED && "No present queue found");


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

	const char** extensionNamesCStr = scratch.alloc<const char*>(extensions.size);
	for (i32 i = 0; i < extensions.size; i++) {
		extensionNamesCStr[i] = extensions[i].data;
	}

	VkDeviceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.queueCreateInfoCount = graphicsQueueFamilyIndex != presentQueueFamilyIndex ? 2u : 1u,
		.pQueueCreateInfos = queueCreateInfos,
		.enabledExtensionCount = (u32)extensions.size,
		.ppEnabledExtensionNames = extensionNamesCStr,
	};
	VkDevice device{ VK_NULL_HANDLE };
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);

	// get queues from the device
	VkQueue graphicsQueue{ VK_NULL_HANDLE };
	VkQueue presentQueue{ VK_NULL_HANDLE };
	vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
	if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
	{
		vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
	}
	else
	{
		presentQueue = graphicsQueue;
	}

	return device;
}


struct VulkanDevice
{
	VkInstance instance{ VK_NULL_HANDLE };
	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
	VkDevice device{ VK_NULL_HANDLE };

	VkQueue graphicsQueue{ VK_NULL_HANDLE };
	VkQueue presentQueue{ VK_NULL_HANDLE };

	VulkanDevice() noexcept = default;

	void init(Arena scatch);
	void destroy();

};

void VulkanDevice::init(Arena scratch) 
{
	instance = getVkInstance();
	physicalDevice = selectPhysicalDevice(instance, scratch);
	device = createLogicalDevice(instance, physicalDevice, { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME, VK_EXT_SHADER_OBJECT_EXTENSION_NAME }, scratch);
}

void VulkanDevice::destroy() 
{
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

// vulkan device
VulkanDevice* device;


void Aurora::Renderer::init(const RendererCreateInfo& rendererCreateInfo, Arena* persistane, Arena scratch)
{
	device = persistane->alloc<VulkanDevice>();
	device->init(scratch);
}

void Aurora::Renderer::RegisterWindow(addptr window)
{

}

void Aurora::Renderer::shutdown()
{
	device->destroy();
}

const char* toConstChar(VkResult result)
{
	return string_VkResult(result);
}
