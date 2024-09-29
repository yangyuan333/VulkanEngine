#include "FrameBuffer.h"
#include "../Render/RenderBackend.h"

#include <stdexcept>

namespace VulkanEngine
{
	FrameBuffer::FrameBuffer(RenderPass const& renderPass, std::vector<std::reference_wrapper<const Image>> const& images)
		:m_renderPass(renderPass)
	{
		m_attachmentImage.resize(m_renderPass.get().GetAttachmentList().size());
		if (images.size() == 0) return;
		if (images.size() != m_renderPass.get().GetAttachmentList().size()) {
			throw std::runtime_error("attachment image mismatch!");
		}
		m_renderPass.get().UpdatePassTextureDescsWidthHeight(images[0].get().GetWidth(), images[0].get().GetHeight());
		m_width = images[0].get().GetWidth();
		m_height = images[0].get().GetHeight();
		int index = 0;
		for (auto const& image : images) BindAttachment(index++, image);
		Build();
	}

	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}

	FrameBuffer::FrameBuffer(FrameBuffer&& other)
		:m_renderPass(other.m_renderPass)
	{
		m_frameBuffer = other.m_frameBuffer;
		m_renderPass = other.m_renderPass;
		m_attachmentImage = other.m_attachmentImage;

		other.m_frameBuffer = VK_NULL_HANDLE;
		other.m_attachmentImage.clear();
	}

	FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other)
	{
		Destroy();

		m_frameBuffer = other.m_frameBuffer;
		m_renderPass = other.m_renderPass;
		m_attachmentImage = other.m_attachmentImage;

		other.m_frameBuffer = VK_NULL_HANDLE;
		other.m_attachmentImage.clear();

		return *this;
	}

	void FrameBuffer::BindAttachment(std::string const& attachmentName, const Image& image)
	{
		m_renderPass.get().UpdatePassTextureDescsWidthHeight(image.GetWidth(), image.GetHeight());
		m_width = image.GetWidth();
		m_height = image.GetHeight();
		m_attachmentImage[m_renderPass.get().GetAttachmentId(attachmentName)] = image.GetImageView();
	}

	void FrameBuffer::BindAttachment(uint32_t attachmentId, const Image& image)
	{
		m_renderPass.get().UpdatePassTextureDescsWidthHeight(image.GetWidth(), image.GetHeight());
		m_width = image.GetWidth();
		m_height = image.GetHeight();
		m_attachmentImage[attachmentId] = image.GetImageView();
	}

	void FrameBuffer::Build()
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass.get().GetRenderPassHandle();
		framebufferInfo.attachmentCount = m_attachmentImage.size();
		framebufferInfo.pAttachments = m_attachmentImage.data();
		framebufferInfo.width = m_width;
		framebufferInfo.height = m_height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(RenderBackend::GetInstance().GetDevice(), &framebufferInfo, nullptr, &m_frameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
	void FrameBuffer::UnBuild()
	{
		m_frameBuffer = VK_NULL_HANDLE;
		m_attachmentImage.clear();
		m_width = 0;
		m_height = 0;
	}
	void FrameBuffer::Destroy()
	{
		if (m_frameBuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(RenderBackend::GetInstance().GetDevice(), m_frameBuffer, nullptr);
			m_frameBuffer = VK_NULL_HANDLE;
			m_attachmentImage.clear();
		}
	}
}