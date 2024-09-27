#pragma once

#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	class Buffer
	{
	private:
		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;
	};
}