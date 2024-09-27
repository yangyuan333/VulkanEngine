#include "Descriptor.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{ 
	VkDescriptorPool CreatePool(VkDevice device, const PoolSizes& poolSizes, int count)
	{
		VkDescriptorPool descriptorPool;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.sizes.size();
		poolInfo.pPoolSizes = poolSizes.sizes.data();
		poolInfo.maxSets = count;
		poolInfo.flags = 0;

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		return descriptorPool;
	}

	void DescriptorAllocator::Init(VkDevice device)
	{
		m_device = device;
		// 其实不应该做这一步，Pool 的 size 应该由Renderer中的RenderPass决定
		// v2.0 再优化吧
		m_currentPool = CreatePool(
			m_device, Config::GetInstance().defaultPoolSize, 
			Config::GetInstance().defaultMaxSets * Config::MAX_FRAMES_IN_FLIGHT);
	}
	void DescriptorAllocator::CleanUp()
	{
		vkDestroyDescriptorPool(m_device, m_currentPool, nullptr);
	}
	std::vector<VkDescriptorSet >DescriptorAllocator::Allocate(std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts)
    {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_currentPool;
		allocInfo.descriptorSetCount = descriptorSetLayouts.size();
		allocInfo.pSetLayouts = descriptorSetLayouts.data();

		std::vector<VkDescriptorSet> descriptorSets;
		descriptorSets.resize(descriptorSetLayouts.size());
		if (vkAllocateDescriptorSets(RenderBackend::GetInstance().GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		
		return descriptorSets;
    }
	VkDescriptorSet DescriptorAllocator::Allocate(VkDescriptorSetLayout const& descriptorSetLayout)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_currentPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		VkDescriptorSet descriptorSet;
		if (vkAllocateDescriptorSets(RenderBackend::GetInstance().GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		return descriptorSet;
	}
}