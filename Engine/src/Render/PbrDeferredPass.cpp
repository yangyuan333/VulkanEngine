#include "PbrDeferredPass.h"
#include "RenderBackend.h"
#include "../Utility/Config.h"
#include <array>

namespace VulkanEngine
{
	const std::unordered_map<std::string, TextureDesc>& PbrDeferredPass::GetPassTextureDescs() 
	{
		return m_pbrDeferredPassTextureDescs;
	}
	void PbrDeferredPass::UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height) const
	{
		for (auto& mapIt : m_pbrDeferredPassTextureDescs)
		{
			mapIt.second.width = width;
			mapIt.second.height = height;
		}
	}
	void PbrDeferredPass::Build()
	{
		// 创建 RenderPass Renderpipeline
		// 延迟渲染分两个 subpass；
		// Subpass-1: G-Buffer --- 四个 G-Buffer
		// G-Buffer-A：Position+Depth
		DeclareColorAttachment(
			"GBufferA",
			m_pbrDeferredPassTextureDescs["GBufferA"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		
		// G-Buffer-B：normal
		DeclareColorAttachment(
			"GBufferB",
			m_pbrDeferredPassTextureDescs["GBufferB"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		// G-Buffer-C：albedo
		DeclareColorAttachment(
			"GBufferC",
			m_pbrDeferredPassTextureDescs["GBufferC"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		// G-Buffer-D：metallicRoughness
		DeclareColorAttachment(
			"GBufferD",
			m_pbrDeferredPassTextureDescs["GBufferD"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);

		// GeometryDepth: 这个深度是得保留的
		DeclareDepthAttachment(
			"SceneDepth",
			m_pbrDeferredPassTextureDescs["SceneDepth"],
				TextureOps{
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE
			},
			// 查一下，看看他们怎么用的，有后处理的才会考虑这里；
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL // 后面可以尝试下VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL这种布局，配合 inputattachment使用;
		);

		// Subpass-2: Lighting --- 全屏后处理
		DeclareColorAttachment(
			"SceneColor",
			m_pbrDeferredPassTextureDescs["SceneColor"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);

		std::array<VkAttachmentReference, 4> subpass1_colorAttachmentRefs;
		subpass1_colorAttachmentRefs[0].attachment = 0,
		subpass1_colorAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		subpass1_colorAttachmentRefs[1].attachment = 1,
		subpass1_colorAttachmentRefs[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		subpass1_colorAttachmentRefs[2].attachment = 2,
		subpass1_colorAttachmentRefs[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		subpass1_colorAttachmentRefs[3].attachment = 3,
		subpass1_colorAttachmentRefs[3].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 4;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkAttachmentReference subpass2_colorAttachmentRef;
		subpass2_colorAttachmentRef.attachment = 5;
		subpass2_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkSubpassDescription> subpasses; subpasses.resize(2);
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 4;
		subpasses[0].pColorAttachments = subpass1_colorAttachmentRefs.data();
		subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;

		subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[1].colorAttachmentCount = 1;
		subpasses[1].pColorAttachments = &subpass2_colorAttachmentRef;
		subpasses[1].inputAttachmentCount = 4;
		subpasses[1].pInputAttachments = subpass1_colorAttachmentRefs.data();

		std::vector<VkSubpassDependency> dependency; dependency.resize(2);// 这里可能得检查一下
		dependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency[0].dstSubpass = 0;
		dependency[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency[0].srcAccessMask = 0;
		dependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		dependency[1].srcSubpass = 0;
		dependency[1].dstSubpass = 1;
		dependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency[1].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// std::vector<VkAttachmentDescription> attachments{ m_colorAttachmentDescriptions };
		// attachments.push_back(m_depthAttachmentDescription);

		CreateVkRenderPass(m_AttachmentDescriptions, subpasses, dependency);

		// Pipeline 也得 build; 最好是得有名字
		m_pipelines.push_back(
			std::make_shared<Pipeline>(
				lighting_vert_spir_path, lighting_frag_spir_path,
				RenderBackend::GetInstance().GetMsaaSampleBit(), Config::GetInstance().fullScreenPipelineConfig,
				m_RenderPass, 1));
		m_pipelines.push_back(
			std::make_shared<Pipeline>(
				geometry_vert_spir_path, geometry_frag_spir_path, 
				RenderBackend::GetInstance().GetMsaaSampleBit(), Config::GetInstance().opaqueScenePipelineConfig, 
				m_RenderPass, 0));
	}
}