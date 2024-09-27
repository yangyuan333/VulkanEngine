#include "VirtualFrame.h"
#include "../Render/RenderBackend.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{ 
	void VirtualFrameProvider::Init(size_t frameCount)
	{
		m_virtualFrames.reserve(frameCount);
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = RenderBackend::GetInstance().GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = frameCount;

		std::vector<VkCommandBuffer> commandBuffers; commandBuffers.resize(frameCount);
		if (vkAllocateCommandBuffers(RenderBackend::GetInstance().GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < frameCount; ++i)
		{
			VkFence fence;
			VkSemaphore imageSemaphore;
			VkSemaphore renderSemaphore;
			if (vkCreateSemaphore(RenderBackend::GetInstance().GetDevice(), &semaphoreInfo, nullptr, &imageSemaphore) != VK_SUCCESS ||
				vkCreateSemaphore(RenderBackend::GetInstance().GetDevice(), &semaphoreInfo, nullptr, &renderSemaphore) != VK_SUCCESS ||
				vkCreateFence(RenderBackend::GetInstance().GetDevice(), &fenceInfo, nullptr, &fence) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphores!");
			}
			
			m_virtualFrames.push_back(VirtualFrame{ commandBuffers[i],fence,imageSemaphore,renderSemaphore });
		}
	}
	void VirtualFrameProvider::Destroy()
	{
		for (auto& frame : m_virtualFrames)
		{
			vkDestroyFence(RenderBackend::GetInstance().GetDevice(), frame.CommandQueueFence, nullptr);
			vkDestroySemaphore(RenderBackend::GetInstance().GetDevice(), frame.imageAvailableSemaphores, nullptr);
			vkDestroySemaphore(RenderBackend::GetInstance().GetDevice(), frame.renderFinishedSemaphores, nullptr);
		}
		m_virtualFrames.clear();
	}
}