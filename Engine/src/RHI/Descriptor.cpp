#include "Descriptor.h"
#include "../Utility/Config.h"

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
}