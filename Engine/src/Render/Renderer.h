#pragma once
#include "../RHI/RenderPass.h"
#include <string>
#include <map>
#include "../Utility/Config.h"

namespace VulkanEngine
{
	class Renderer
	{
	public:
		virtual void AddRenderPass(RenderPassEnum renderPass);
	public:
		std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
	};
}