#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.h"

namespace VulkanEngine {

	struct PoolSizes { std::vector<VkDescriptorPoolSize> sizes; };

	class DescriptorAllocator 
	{
	public:
		void Init(VkDevice device);
		void CleanUp();

		std::vector<VkDescriptorSet> Allocate(std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts);
		VkDescriptorSet Allocate(VkDescriptorSetLayout const& descriptorSetLayout);

	private:
		VkDevice m_device;
		VkDescriptorPool m_currentPool{ nullptr };
	};

} // namespace wind