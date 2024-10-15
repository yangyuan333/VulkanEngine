#pragma once
#include "Pipeline.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace VulkanEngine
{
	class FrameBuffer;
	class GameObject;

	struct TextureOps {
		VkAttachmentLoadOp  load = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp store = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkAttachmentLoadOp  stencilLoad = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp stencilStore = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	};

	class RenderPass
	{
	public:
		RenderPass() = default;
		~RenderPass();
		RenderPass(RenderPass const& other) = delete;
		RenderPass& operator=(RenderPass const& other) = delete;
		RenderPass(RenderPass&& other);
		RenderPass& operator=(RenderPass&& other);
	public:
		virtual void Build() = 0;
		virtual const std::unordered_map<std::string, TextureDesc>& GetPassTextureDescs() = 0;
		virtual void UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height) const = 0;
		virtual std::vector<std::map<int, VkDescriptorSet>>& BindGameObject(std::shared_ptr<GameObject> object) = 0;
		inline VkRenderPass GetRenderPassHandle() const { return m_RenderPass; }
		inline std::vector<VkAttachmentDescription> GetAttachmentList() const { return m_AttachmentDescriptions; }
		inline std::map<std::string, VkAttachmentDescription> GetAttachmentMap() const { return m_name2AttachmentDescription; }
		uint32_t GetAttachmentId(std::string attachmenName) const;
		std::shared_ptr<FrameBuffer> GetFameBuffer() const { return m_frameBuffer; }
		std::vector<VkClearValue> GetClearColor() const { return m_AttachmentClearValue; }
		void BindFrameBuffer(std::shared_ptr<FrameBuffer> frameBuffer);
		void UnBindFrameBuffer() { m_frameBuffer  = nullptr; }
		std::vector<std::shared_ptr<Pipeline>>& GetPipelines() { return m_pipelines; }
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
		void Destroy();
	protected:
		std::string m_PassName;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<std::shared_ptr<Pipeline>> m_pipelines; // subpass 目前只针对 opaque 物体进行渲染，创建一套即可
		std::shared_ptr<FrameBuffer> m_frameBuffer;

		std::vector<VkAttachmentDescription> m_AttachmentDescriptions;
		std::vector<VkClearValue> m_AttachmentClearValue;
		std::map<std::string, VkAttachmentDescription> m_name2AttachmentDescription;
		std::map<std::string, uint32_t> m_name2Id;
		// std::vector<VkAttachmentDescription> m_colorAttachmentDescriptions;
		// VkAttachmentDescription m_depthAttachmentDescription;
		// std::vector<VkClearValue> m_colorAttachmentClearValue;
		// VkClearValue m_depthAttachmentClearValue;

		
	};
}