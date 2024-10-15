#include "DeferRenderer.h"
#include "../Utility/Config.h"
#include "RenderBackend.h"
#include "../RHI/FrameBuffer.h"

namespace VulkanEngine
{
	DeferredRenderer::DeferredRenderer()
	{
		Init();
	}
	DeferredRenderer::~DeferredRenderer()
	{
		
	}
	void DeferredRenderer::Render(Scene& scene)
	{
		
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
	}
}