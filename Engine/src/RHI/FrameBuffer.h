#pragma once

#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	class FrameBuffer
	{
	public:
		inline VkFramebuffer GetFrameBufferHandle() const { return m_frameBuffer; }
	private:
		VkFramebuffer m_frameBuffer;
	};
}