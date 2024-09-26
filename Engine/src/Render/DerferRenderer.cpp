#include "DerferRenderer.h"

namespace VulkanEngine
{
	void DeferredRenderer::Render(Scene& scene)
	{
		
	}
	void DeferredRenderer::Init()
	{
		// 创建 RenderPass 以及对应的 Pipeline 以及对应的 FrameBuffer;

		AddRenderPass(RenderPassEnum::PbrDeferredPass);

	}
}