#include "Renderer.h"

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
				
				break;
			}
		}
	}
}