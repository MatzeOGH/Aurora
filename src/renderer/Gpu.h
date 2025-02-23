#pragma once

#include "auroravulkan.h"

#include "core/arena.h"
#include "core/span.h"

namespace Aurora {

	struct Gpu {
		VkInstance instance{VK_NULL_HANDLE};
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
		VkDevice device{ VK_NULL_HANDLE };

		u32 graphicsQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
		u32 presentQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };

		VkQueue graphicsQueue{ VK_NULL_HANDLE };
		VkQueue presentQueue{ VK_NULL_HANDLE };

		void init(Span<String> deviceExtensions, Arena scratch);
		void shutdown(Arena scratch);
	};

}