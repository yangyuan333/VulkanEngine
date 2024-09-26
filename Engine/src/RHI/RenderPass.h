#pragma once
#include "Pipeline.h"

namespace VulkanEngine
{
	class RenderPass
	{
	public:
		virtual void Build();
	private:
		std::string PassName;
		VkRenderPass RenderPass;
		std::vector<Pipeline> pipelines; // subpass
	};
}