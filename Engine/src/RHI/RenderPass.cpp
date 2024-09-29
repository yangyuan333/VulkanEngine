#include "RenderPass.h"
#include "../Render/RenderBackend.h"

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
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// memory optim
		colorAttachment.initialLayout = initialLayout;
		colorAttachment.finalLayout = finalLayout;

		// m_colorAttachmentDescriptions.push_back(colorAttachment);
		
		m_AttachmentDescriptions.push_back(colorAttachment);
		m_name2AttachmentDescription[name] = colorAttachment;

		VkClearValue temp;
		temp.color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
		m_AttachmentClearValue.push_back(temp);
	}

	void RenderPass::DeclareDepthAttachment(const std::string& name, const TextureDesc& textureDesc, const TextureOps& ops, VkImageLayout initialLayout, VkImageLayout finalLayout, ClearDepthStencil clearDepthStencil)
	{
		VkAttachmentDescription Attachment = {};
		Attachment.format = textureDesc.format;
		Attachment.samples = textureDesc.sampleCount;
		// color and depth
		Attachment.loadOp = ops.load;
		Attachment.storeOp = ops.store;
		// stencil
		Attachment.stencilLoadOp = ops.stencilLoad;
		Attachment.stencilStoreOp = ops.stencilStore;
		// memory optim
		Attachment.initialLayout = initialLayout;
		Attachment.finalLayout = finalLayout;

		m_AttachmentDescriptions.push_back(Attachment);
		m_name2AttachmentDescription[name] = Attachment;

		VkClearValue temp;
		temp.depthStencil = { clearDepthStencil.depth, clearDepthStencil.stencil };
		m_AttachmentClearValue.push_back(temp);
	}

	void RenderPass::CreateVkRenderPass(
		const std::vector<VkAttachmentDescription>& attachments, 
		const std::vector<VkSubpassDescription>& subpasses, 
		const std::vector<VkSubpassDependency>& dependency)
	{
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = subpasses.size();
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.dependencyCount = dependency.size();
		renderPassInfo.pDependencies = dependency.data();
	}

    RenderPass::~RenderPass()
    {
		Destroy();
    }

    RenderPass::RenderPass(RenderPass&& other)
    {
		m_PassName = other.m_PassName;
		m_RenderPass = other.m_RenderPass;
		m_pipelines = other.m_pipelines;
		m_frameBuffer = other.m_frameBuffer;

		m_AttachmentDescriptions = other.m_AttachmentDescriptions;
		m_AttachmentClearValue = other.m_AttachmentClearValue;
		m_name2AttachmentDescription = other.m_name2AttachmentDescription;

		// m_colorAttachmentDescriptions = other.m_colorAttachmentDescriptions;
		// m_depthAttachmentDescription = other.m_depthAttachmentDescription;
		// m_colorAttachmentClearValue = other.m_colorAttachmentClearValue;
		// m_depthAttachmentClearValue = other.m_depthAttachmentClearValue;

		other.m_RenderPass = VK_NULL_HANDLE;
		other.m_pipelines.clear();
		other.m_frameBuffer = nullptr;
    }

    RenderPass& RenderPass::operator=(RenderPass&& other)
    {
        // TODO: 在此处插入 return 语句
		Destroy();

		m_PassName = other.m_PassName;
		m_RenderPass = other.m_RenderPass;
		m_pipelines = other.m_pipelines;
		m_frameBuffer = other.m_frameBuffer;

		m_AttachmentDescriptions = other.m_AttachmentDescriptions;
		m_AttachmentClearValue = other.m_AttachmentClearValue;
		m_name2AttachmentDescription = other.m_name2AttachmentDescription;

		// m_colorAttachmentDescriptions = other.m_colorAttachmentDescriptions;
		// m_depthAttachmentDescription = other.m_depthAttachmentDescription;
		// m_colorAttachmentClearValue = other.m_colorAttachmentClearValue;
		// m_depthAttachmentClearValue = other.m_depthAttachmentClearValue;

		other.m_RenderPass = VK_NULL_HANDLE;
		other.m_pipelines.clear();
		other.m_frameBuffer = nullptr;

		return *this;
    }

	void RenderPass::Destroy()
	{
		if (m_RenderPass != VK_NULL_HANDLE)
		{
			m_pipelines.clear();
			vkDestroyRenderPass(RenderBackend::GetInstance().GetDevice(), m_RenderPass, nullptr);
			m_frameBuffer = nullptr;
			m_RenderPass = VK_NULL_HANDLE;

			m_AttachmentDescriptions.clear();
			m_AttachmentClearValue.clear();
			m_name2AttachmentDescription.clear();
		}
	}
}