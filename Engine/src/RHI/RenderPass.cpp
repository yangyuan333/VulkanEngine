#include "RenderPass.h"

namespace VulkanEngine
{
	void RenderPass::DeclareColorAttachment(const std::string& name, const TextureDesc& textureDesc, const TextureOps& ops, VkImageLayout initialLayout, VkImageLayout finalLayout, ClearColor clearColor)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = textureDesc.format;
		colorAttachment.samples = textureDesc.sampleCount;
		// color and depth
		colorAttachment.loadOp = ops.load;
		colorAttachment.storeOp = ops.store;
		// stencil
		colorAttachment.stencilLoadOp = ops.stencilLoad;
		colorAttachment.stencilStoreOp = ops.stencilStore;
		// memory optim
		colorAttachment.initialLayout = initialLayout;
		colorAttachment.finalLayout = finalLayout;

		m_colorAttachmentDescriptions.push_back(colorAttachment);

		VkClearValue temp;
		temp.color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
		m_colorAttachmentClearValue.push_back(temp);
	}

	void RenderPass::DeclareDepthAttachment(const std::string& name, const TextureDesc& textureDesc, const TextureOps& ops, VkImageLayout initialLayout, VkImageLayout finalLayout, ClearDepthStencil clearDepthStencil)
	{
		m_depthAttachmentDescription.format = textureDesc.format;
		m_depthAttachmentDescription.samples = textureDesc.sampleCount;
		// color and depth
		m_depthAttachmentDescription.loadOp = ops.load;
		m_depthAttachmentDescription.storeOp = ops.store;
		// stencil
		m_depthAttachmentDescription.stencilLoadOp = ops.stencilLoad;
		m_depthAttachmentDescription.stencilStoreOp = ops.stencilStore;
		// memory optim
		m_depthAttachmentDescription.initialLayout = initialLayout;
		m_depthAttachmentDescription.finalLayout = finalLayout;

		m_depthAttachmentClearValue.depthStencil = { clearDepthStencil.depth, clearDepthStencil.stencil };
	}
}