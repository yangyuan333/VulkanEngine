#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RenderPass.h"
#include "Image.h"

namespace VulkanEngine
{
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		~FrameBuffer();

		void BindAttachment(const Image& image);
		void Build();
	public:
		inline VkFramebuffer GetFrameBufferHandle() const { return m_frameBuffer; }
	private:
		VkFramebuffer m_frameBuffer;
		std::vector<VkImageView> m_attachmentImage; 
	};
}