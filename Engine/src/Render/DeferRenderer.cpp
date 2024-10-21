#include "DeferRenderer.h"
#include "../Utility/Config.h"
#include "RenderBackend.h"
#include "../RHI/FrameBuffer.h"

namespace VulkanEngine
{
	// 选哪个呢？
	DeferredRenderer::DeferredRenderer()
	{
		Init();
	}
	DeferredRenderer::~DeferredRenderer()
	{
		
	}
	void DeferredRenderer::Render(Scene& scene)
	{
		// 渲染主流程了
		auto& gameObjects = scene.GetGameObject();
		// PbrDeferredPass---目前只有这一个Pass
		if (!m_renderPasses.count(RenderPassEnum::PbrDeferredPass))
		{
			throw std::runtime_error("there is not pbr deferredpass!");
		}
		auto pbrPass = m_renderPasses[RenderPassEnum::PbrDeferredPass];
		pbrPass->BeginRenderPass(*m_frameBuffers[RenderPassEnum::PbrDeferredPass][RenderBackend::GetInstance().GetCurrentFrameIndex()]);

		pbrPass->BindPipeline(0);
		RenderBackend::GetInstance().SetDynamicViewportScissor();
		// 相机、灯光 descriptorset 绑定；
		scene.GetCamera()->BindDescriptorSet(pbrPass->GetPipelines()[0]->GetPipelineLayout(), 1);
		for (auto& gameObject : gameObjects)
		{
			gameObject->BindVertex();
			gameObject->BindIndex();
			// model 矩阵在这里动态更新
			// gameObject->UpdateTransform(); 这里后续涉及到动态物体时 需要更新；
			gameObject->BindDescriptorSet(MaterialType::DeferredPassMaterial, 0);
			gameObject->Draw();
		}

		pbrPass->BindPipeline(1);
		scene.BindLightsDescriptorSet(pbrPass->GetPipelines()[1]->GetPipelineLayout(), 0);
		scene.GetCamera()->BindDescriptorSet(pbrPass->GetPipelines()[1]->GetPipelineLayout(), 1);
		pbrPass->BindInputDescriptorSet(pbrPass->GetPipelines()[1]->GetPipelineLayout(), 2);

		vkCmdDraw(RenderBackend::GetInstance().GetCurrentFrame().Commands.GetCommandBufferHandle(),3,1,0,0);

		pbrPass->EndRenderPass();
	}
	void DeferredRenderer::CreateFrameBuffer()
	{
		CreatePassImageViews();
		std::vector<std::shared_ptr<FrameBuffer>> pbrFrameBuffer;
		for (auto swapChainImage : RenderBackend::GetInstance().GetSwapChainImages())
		{
			std::vector<std::reference_wrapper<const Image>> pbrImages;
			pbrImages.push_back(*m_images[ImageEnum::GBufferA]);
			pbrImages.push_back(*m_images[ImageEnum::GBufferB]);
			pbrImages.push_back(*m_images[ImageEnum::GBufferC]);
			pbrImages.push_back(*m_images[ImageEnum::GBufferD]);
			pbrImages.push_back(*m_images[ImageEnum::Depth]);
			pbrImages.push_back(*swapChainImage);

			pbrFrameBuffer.push_back(std::make_shared<FrameBuffer>(*(m_renderPasses[RenderPassEnum::PbrDeferredPass]), pbrImages));
		}
		m_frameBuffers[RenderPassEnum::PbrDeferredPass] = pbrFrameBuffer;

		// PBR DeferredPass
		auto& inputDescritorSets = m_renderPasses[RenderPassEnum::PbrDeferredPass]->GetInputAttachmentDescriptorSet();
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			VkDescriptorImageInfo imageInfos[4];
			imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos[0].imageView = m_images[ImageEnum::GBufferA]->GetImageView();
			imageInfos[0].sampler = RenderBackend::GetInstance().GetSampler()->GetSamplerHandle();

			imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos[1].imageView = m_images[ImageEnum::GBufferB]->GetImageView();
			imageInfos[1].sampler = RenderBackend::GetInstance().GetSampler()->GetSamplerHandle();

			imageInfos[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos[2].imageView = m_images[ImageEnum::GBufferC]->GetImageView();
			imageInfos[2].sampler = RenderBackend::GetInstance().GetSampler()->GetSamplerHandle();

			imageInfos[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos[3].imageView = m_images[ImageEnum::GBufferD]->GetImageView();
			imageInfos[3].sampler = RenderBackend::GetInstance().GetSampler()->GetSamplerHandle();

			std::vector<VkWriteDescriptorSet> descriptorWrites; descriptorWrites.resize(4);
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = inputDescritorSets[frameIdx];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = nullptr;
			descriptorWrites[0].pImageInfo = imageInfos; // Optional
			descriptorWrites[0].pTexelBufferView = nullptr; // Optional

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = inputDescritorSets[frameIdx];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = nullptr;
			descriptorWrites[1].pImageInfo = &imageInfos[1]; // Optional
			descriptorWrites[1].pTexelBufferView = nullptr; // Optional

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = inputDescritorSets[frameIdx];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = nullptr;
			descriptorWrites[2].pImageInfo = &imageInfos[2]; // Optional
			descriptorWrites[2].pTexelBufferView = nullptr; // Optional

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = inputDescritorSets[frameIdx];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pBufferInfo = nullptr;
			descriptorWrites[3].pImageInfo = &imageInfos[3]; // Optional
			descriptorWrites[3].pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(
				RenderBackend::GetInstance().GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}
	void DeferredRenderer::RecreateFrameBuffer()
	{
		// Step 1: Clear the FrameBuffers and Images
		m_images.clear();
		m_frameBuffers.clear();
		// Step 2: Create the FrameBuffers and Images
		CreateFrameBuffer();
	}
	void DeferredRenderer::CreatePassImageViews()
	{
		m_images.clear();
		auto textureDesc = m_renderPasses[RenderPassEnum::PbrDeferredPass]->GetPassTextureDescs();
		m_images[ImageEnum::Depth] = std::make_shared<Image>(
			textureDesc["SceneDepth"].width, textureDesc["SceneDepth"].height, textureDesc["SceneDepth"].format,
			textureDesc["SceneDepth"].usage, textureDesc["SceneDepth"].memoryUsage, 
			ImageOptions::DEFAULT
		);
		m_images[ImageEnum::GBufferA] = std::make_shared<Image>(
			textureDesc["GBufferA"].width, textureDesc["GBufferA"].height, textureDesc["GBufferA"].format,
			textureDesc["GBufferA"].usage, textureDesc["GBufferA"].memoryUsage,
			ImageOptions::DEFAULT
		);
		m_images[ImageEnum::GBufferB] = std::make_shared<Image>(
			textureDesc["GBufferB"].width, textureDesc["GBufferB"].height, textureDesc["GBufferB"].format,
			textureDesc["GBufferB"].usage, textureDesc["GBufferB"].memoryUsage,
			ImageOptions::DEFAULT);
		m_images[ImageEnum::GBufferC] = std::make_shared<Image>(
			textureDesc["GBufferC"].width, textureDesc["GBufferC"].height, textureDesc["GBufferC"].format,
			textureDesc["GBufferC"].usage, textureDesc["GBufferC"].memoryUsage,
			ImageOptions::DEFAULT);
		m_images[ImageEnum::GBufferD] = std::make_shared<Image>(
			textureDesc["GBufferD"].width, textureDesc["GBufferD"].height, textureDesc["GBufferD"].format,
			textureDesc["GBufferD"].usage, textureDesc["GBufferD"].memoryUsage,
			ImageOptions::DEFAULT);
	}
	void DeferredRenderer::Init()
	{
		// 创建 RenderPass 以及对应的 Pipeline 以及对应的 FrameBuffer;

		AddRenderPass(RenderPassEnum::PbrDeferredPass);
		// 创建对应的imageview；组建对应的framebuffer；底层的image应该是不用double的，因为pipeline有barrier；
		// ImageView所需要：Depth Image View；SwapChain的放到它那边做；G-Buffer的ImageView；
		CreateFrameBuffer();
	}
}