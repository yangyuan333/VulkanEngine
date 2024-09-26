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
	const void PbrDeferredPass::UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height)
	{
		for (auto& mapIt : m_pbrDeferredPassTextureDescs)
		{
			mapIt.second.width = width;
			mapIt.second.height = height;
		}
	}
	void PbrDeferredPass::Build()
	{
		// ���� RenderPass Renderpipeline
		// �ӳ���Ⱦ������ subpass��
		// Subpass-1: G-Buffer --- �ĸ� G-Buffer
		// G-Buffer-A��Position+Depth
		DeclareColorAttachment(
			"GBufferA",
			m_pbrDeferredPassTextureDescs["GBufferA"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // ֮�������ԣ�����Ӧ�ò���Ҫ store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		
		// G-Buffer-B��normal
		DeclareColorAttachment(
			"GBufferB",
			m_pbrDeferredPassTextureDescs["GBufferB"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // ֮�������ԣ�����Ӧ�ò���Ҫ store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		// G-Buffer-C��albedo
		DeclareColorAttachment(
			"GBufferC",
			m_pbrDeferredPassTextureDescs["GBufferC"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // ֮�������ԣ�����Ӧ�ò���Ҫ store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);
		// G-Buffer-D��metallicRoughness
		DeclareColorAttachment(
			"GBufferD",
			m_pbrDeferredPassTextureDescs["GBufferD"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // ֮�������ԣ�����Ӧ�ò���Ҫ store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED);

		// GeometryDepth: �������ǵñ�����
		DeclareDepthAttachment(
			"SceneDepth",
			m_pbrDeferredPassTextureDescs["SceneDepth"],
				TextureOps{
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE
			},
			// ��һ�£�����������ô�õģ��к����ĲŻῼ�����
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL // ������Գ�����VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL���ֲ��֣���� inputattachmentʹ��;
		);

		// Subpass-2: Lighting --- ȫ������
		DeclareColorAttachment(
			"SceneColor",
			m_pbrDeferredPassTextureDescs["SceneColor"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, // ֮�������ԣ�����Ӧ�ò���Ҫ store
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
		depthAttachmentRef.attachment = 5;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::array<VkSubpassDescription, 2> subpasses;
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 4;
		subpasses[0].pColorAttachments = subpass1_colorAttachmentRefs.data();
		subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;

		subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[1].colorAttachmentCount = 1;
		subpasses[1].pColorAttachments = &subpass2_colorAttachmentRef;
		subpasses[1].inputAttachmentCount = 4;
		subpasses[1].pInputAttachments = subpass1_colorAttachmentRefs.data();

		std::array<VkSubpassDependency, 2> dependency; // ������ܵü��һ��
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


	}
}