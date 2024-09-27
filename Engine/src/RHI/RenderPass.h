#pragma once
#include "Pipeline.h"
#include "../Render/RenderResource.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace VulkanEngine
{
	struct TextureOps {
		VkAttachmentLoadOp  load = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp store = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkAttachmentLoadOp  stencilLoad = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp stencilStore = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	};

	class RenderPass
	{
	public:
		virtual void Build() = 0;
		virtual const std::unordered_map<std::string, TextureDesc>& GetPassTextureDescs() = 0;
		virtual const void UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height) = 0;
		inline VkRenderPass GetRenderPassHandle() const { return m_RenderPass; }
		inline VkRenderPass GetRenderPassHandle() { return m_RenderPass; }
	protected:
		void DeclareColorAttachment(
			const std::string& name, const TextureDesc& textureDesc,
			const TextureOps& ops, VkImageLayout initialLayout,
			VkImageLayout finalLayout, ClearColor clearColor = ClearColor{});
		void DeclareDepthAttachment(
			const std::string& name, const TextureDesc& textureDesc,
			const TextureOps& ops, VkImageLayout initialLayout,
			VkImageLayout   finalLayout,
			ClearDepthStencil clearDepthStencil = ClearDepthStencil{});
		void CreateVkRenderPass(
			const std::vector<VkAttachmentDescription>& attachments, 
			const std::vector<VkSubpassDescription>& subpasses,
			const std::vector<VkSubpassDependency>& dependency);
	protected:
		std::string m_PassName;
		VkRenderPass m_RenderPass;
		std::vector<Pipeline> m_pipelines; // subpass 目前只针对 opaque 物体进行渲染，创建一套即可
		VkFramebuffer m_frameBuffer;
		std::vector<VkAttachmentDescription> m_colorAttachmentDescriptions;
		VkAttachmentDescription m_depthAttachmentDescription;
		std::vector<VkClearValue> m_colorAttachmentClearValue;
		VkClearValue m_depthAttachmentClearValue;
	};
}