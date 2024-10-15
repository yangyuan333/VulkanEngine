#include "Renderer.h"
#include "PbrDeferredPass.h"

namespace VulkanEngine
{
	Renderer::Renderer()
	{
		;
	}
	Renderer::~Renderer()
	{
		;
	}
	void VulkanEngine::Renderer::AddRenderPass(RenderPassEnum renderPass)
	{
		switch (renderPass)
		{
			case RenderPassEnum::PbrDeferredPass:
			{
				m_renderPasses[renderPass] = std::make_shared<PbrDeferredPass>();
				break;
			}
		}
	}
}