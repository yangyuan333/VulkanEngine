#pragma once

#include <vulkan/vulkan.h>
#include <utility>

namespace VulkanEngine
{
	class CommandBuffer
	{
	public:
		CommandBuffer(VkCommandBuffer commandBuffer) : m_handle(std::move(commandBuffer)) {}

	private:
		VkCommandBuffer m_handle;
	};
}