#include "DerferRenderer.h"

namespace VulkanEngine
{
	void DeferredRenderer::Render(Scene& scene)
	{
		
	}
	void DeferredRenderer::Init()
	{
		// ���� RenderPass �Լ���Ӧ�� Pipeline �Լ���Ӧ�� FrameBuffer;

		AddRenderPass(RenderPassEnum::PbrDeferredPass);

	}
}