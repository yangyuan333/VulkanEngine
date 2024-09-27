#pragma once

#include <memory>
#include <vector>

#include "../RHI/Pipeline.h"

namespace VulkanEngine
{
	class Material
	{
	public:
		virtual void bindDescriptorSet() = 0;
		void SetPipeline(std::vector<Pipeline> const& pipelines) { m_pipelines = pipelines; }
	private:
		std::vector<Pipeline> m_pipelines; // subpass
	};
}
