#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "RenderPass.h"
#include "Image.h"

namespace VulkanEngine
{
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		FrameBuffer(RenderPass const& renderPass, std::vector<std::reference_wrapper<const Image>> const& images = {});
		~FrameBuffer();

		FrameBuffer(FrameBuffer const& other) = delete;
		FrameBuffer& operator=(FrameBuffer const& other) = delete;

		FrameBuffer(FrameBuffer && other);
		FrameBuffer& operator=(FrameBuffer && other);

		void BindAttachment(std::string const& attachmentName, const Image& image);
		void BindAttachment(uint32_t attachmentId, const Image& image);
		void Build();
		void UnBuild();

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
	public:
		inline VkFramebuffer GetFrameBufferHandle() const { return m_frameBuffer; }
	private:
		void Destroy();
	private:
		std::reference_wrapper<const RenderPass> m_renderPass;
		VkFramebuffer m_frameBuffer = VK_NULL_HANDLE;
		std::vector<VkImageView> m_attachmentImage; 
		uint32_t m_width;
		uint32_t m_height;
	};
}