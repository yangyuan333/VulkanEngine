#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanEngine
{
	class Pipeline
	{
	private:
		VkPipeline m_pipeline;
		VkShaderModule m_vertexShader;
		VkShaderModule m_fragShader;
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
	};
}