#pragma once
#include <utility>
#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
}