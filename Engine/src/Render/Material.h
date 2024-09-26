#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "../RHI/Pipeline.h"

namespace VulkanEngine
{
	/// <summary>
	/// 不同的材质具有不同的Pipeline---shader、材质纹理参数等
	/// </summary>
	class Material
	{
	public:
		virtual void bindDescriptorSet() = 0;
		inline void SetPipeline(std::vector<Pipeline>& pipelines) { m_pipelines = pipelines; }
	private:
		std::vector<Pipeline> m_pipelines; // subpass
	};
}