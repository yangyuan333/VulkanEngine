#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "../RHI/Pipeline.h"

namespace VulkanEngine
{
	/// <summary>
	/// ��ͬ�Ĳ��ʾ��в�ͬ��Pipeline---shader���������������
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