#include "PbrDeferredPass.h"
#include "RenderBackend.h"
#include "../Utility/Config.h"
#include <array>

namespace VulkanEngine
{
	PbrDeferredPass::PbrDeferredPass()
	{
		Build();
		m_inputAttachmentSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
		for (int frameIndex = 0; frameIndex < Config::MAX_FRAMES_IN_FLIGHT; ++frameIndex)
		{
			auto allocator = RenderBackend::GetInstance().GetDescriptorAllocator();
			auto layout = m_pipelines[1]->GetDescriptorSetLayout();
			m_inputAttachmentSets[frameIndex] = allocator->Allocate(layout[2]);
			// m_inputAttachmentSets[frameIndex] = RenderBackend::GetInstance().GetDescriptorAllocator()->Allocate(m_pipelines[1]->GetDescriptorSetLayout()[2]);
		}
	}
	PbrDeferredPass::~PbrDeferredPass()
	{
		;
	}
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
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		// G-Buffer-B：normal
		DeclareColorAttachment(
			"GBufferB",
			m_pbrDeferredPassTextureDescs["GBufferB"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		// G-Buffer-C：albedo
		DeclareColorAttachment(
			"GBufferC",
			m_pbrDeferredPassTextureDescs["GBufferC"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		// G-Buffer-D：metallicRoughness
		DeclareColorAttachment(
			"GBufferD",
			m_pbrDeferredPassTextureDescs["GBufferD"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, // 之后来试试，这里应该不需要 store
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// GeometryDepth: 这个深度是得保留的
		DeclareDepthAttachment(
			"SceneDepth",
			m_pbrDeferredPassTextureDescs["SceneDepth"],
				TextureOps{
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE
			},
			// 查一下，看看他们怎么用的，有后处理的才会考虑这里；
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // 后面可以尝试下VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL这种布局，配合 inputattachment使用;
		);

		// Subpass-2: Lighting --- 全屏后处理
		DeclareColorAttachment(
			"SceneColor",
			m_pbrDeferredPassTextureDescs["SceneColor"],
			TextureOps{
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE
			},
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

		std::array<VkAttachmentReference, 4> subpass2_inputAttachmentRefs;
		subpass2_inputAttachmentRefs[0].attachment = 0,
		subpass2_inputAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subpass2_inputAttachmentRefs[1].attachment = 1,
		subpass2_inputAttachmentRefs[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subpass2_inputAttachmentRefs[2].attachment = 2,
		subpass2_inputAttachmentRefs[2].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subpass2_inputAttachmentRefs[3].attachment = 3,
		subpass2_inputAttachmentRefs[3].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkSubpassDescription> subpasses; subpasses.resize(2);
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 4;
		subpasses[0].pColorAttachments = subpass1_colorAttachmentRefs.data();
		subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;

		subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[1].colorAttachmentCount = 1;
		subpasses[1].pColorAttachments = &subpass2_colorAttachmentRef;
		subpasses[1].inputAttachmentCount = 4;
		subpasses[1].pInputAttachments = subpass2_inputAttachmentRefs.data();

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
		dependency[1].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		// std::vector<VkAttachmentDescription> attachments{ m_colorAttachmentDescriptions };
		// attachments.push_back(m_depthAttachmentDescription);

		CreateVkRenderPass(m_AttachmentDescriptions, subpasses, dependency);

		// Pipeline 也得 build; 最好是得有名字
		m_pipelines.push_back(
			std::make_shared<Pipeline>(
				geometry_vert_spir_path, geometry_frag_spir_path, 
				RenderBackend::GetInstance().GetMsaaSampleBit(), Config::GetInstance().opaqueScenePipelineConfig, 
				m_RenderPass, 0));
		m_pipelines.push_back(
			std::make_shared<Pipeline>(
				lighting_vert_spir_path, lighting_frag_spir_path,
				RenderBackend::GetInstance().GetMsaaSampleBit(), Config::GetInstance().fullScreenPipelineConfig,
				m_RenderPass, 1));
	}

	void PbrDeferredPass::BindGameObject(GameObject& object)
    {
		if (object.GetDescriptorSets().count(MaterialType::DeferredPassMaterial) > 0)
			throw std::runtime_error("GameObject DescritorSet Create Already!");
		// std::vector<std::map<int, VkDescriptorSet>> descriptorSet; descriptorSet.resize(Config::MAX_FRAMES_IN_FLIGHT);
		auto& descriptorSet = object.GetDescriptorSets()[MaterialType::DeferredPassMaterial];
		descriptorSet.resize(Config::MAX_FRAMES_IN_FLIGHT);
		if (object.GetGameObjectKind() == GameObjectKind::Opaque || object.GetGameObjectKind() == GameObjectKind::Transparent)
		{
			// 先手动绑吧，自动绑有点麻烦
			// subpass-1: set0 ModelBuffer, albedoTextureSampler, normalTextureSampler, metallicRoughnessTextureSampler
			// subpass-2: no
			for (auto frameIdex = 0; frameIdex < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdex)
			{
				auto& descriptorLayouts = m_pipelines[0]->GetDescriptorSetLayout();
				descriptorSet[frameIdex][0] = (RenderBackend::GetInstance().GetDescriptorAllocator()->Allocate(descriptorLayouts[0]));

				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = object.GetModelBuffers()[frameIdex]->GetBufferHandle();
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(ModelComponent);

				VkDescriptorImageInfo albedoimageInfo{};
				albedoimageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				albedoimageInfo.imageView = object.GetMeshTexture("albedo")->GetImageView();
				albedoimageInfo.sampler = object.GetSampler()->GetSamplerHandle();

				VkDescriptorImageInfo normalimageInfo{};
				normalimageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				normalimageInfo.imageView = object.GetMeshTexture("normal")->GetImageView();
				normalimageInfo.sampler = object.GetSampler()->GetSamplerHandle();

				VkDescriptorImageInfo roughnessMetallicimageInfo{};
				roughnessMetallicimageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				roughnessMetallicimageInfo.imageView = object.GetMeshTexture("metallicRoughness")->GetImageView();
				roughnessMetallicimageInfo.sampler = object.GetSampler()->GetSamplerHandle();

				std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSet[frameIdex][0];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;
				descriptorWrites[0].pImageInfo = nullptr; // Optional
				descriptorWrites[0].pTexelBufferView = nullptr; // Optional

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = descriptorSet[frameIdex][0];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = nullptr;
				descriptorWrites[1].pImageInfo = &albedoimageInfo; // Optional
				descriptorWrites[1].pTexelBufferView = nullptr; // Optional

				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = descriptorSet[frameIdex][0];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pBufferInfo = nullptr;
				descriptorWrites[2].pImageInfo = &normalimageInfo; // Optional
				descriptorWrites[2].pTexelBufferView = nullptr; // Optional

				descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[3].dstSet = descriptorSet[frameIdex][0];
				descriptorWrites[3].dstBinding = 3;
				descriptorWrites[3].dstArrayElement = 0;
				descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[3].descriptorCount = 1;
				descriptorWrites[3].pBufferInfo = nullptr;
				descriptorWrites[3].pImageInfo = &roughnessMetallicimageInfo; // Optional
				descriptorWrites[3].pTexelBufferView = nullptr; // Optional

				vkUpdateDescriptorSets(RenderBackend::GetInstance().GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
			}
		}
	}

	void PbrDeferredPass::BeginRenderPass(FrameBuffer& frameBuffer)
	{
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = frameBuffer.GetFrameBufferHandle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = RenderBackend::GetInstance().GetSwapChainExtent();
		renderPassInfo.clearValueCount = m_AttachmentClearValue.size();
		renderPassInfo.pClearValues = m_AttachmentClearValue.data();
		vkCmdBeginRenderPass(
			RenderBackend::GetInstance().GetCurrentFrame().Commands.GetCommandBufferHandle(), 
			&renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void PbrDeferredPass::EndRenderPass()
	{
		vkCmdEndRenderPass(RenderBackend::GetInstance().GetCurrentFrame().Commands.GetCommandBufferHandle());
	}
	void PbrDeferredPass::BindInputDescriptorSet(VkPipelineLayout pipelineLayout, int setIndex)
	{
		vkCmdBindDescriptorSets(
			RenderBackend::GetInstance().GetCurrentFrame().Commands.GetCommandBufferHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout, setIndex, 1, &m_inputAttachmentSets[RenderBackend::GetInstance().GetCurrentFrameIndex()], 0, nullptr);
	}
}