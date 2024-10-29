#include "VirtualFrame.h"
#include "../Render/RenderBackend.h"
#include "../Utility/Config.h"

#include <array>

namespace VulkanEngine
{ 
	void VirtualFrameProvider::Init(size_t frameCount, size_t stagingBufferSize)
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

			m_virtualFrames.push_back(VirtualFrame{ commandBuffers[i],fence,imageSemaphore,renderSemaphore,StagingBuffer{stagingBufferSize} });
		}
	}

	void VirtualFrameProvider::Destroy()
	{
		for (auto& frame : m_virtualFrames)
		{
			vkDestroyFence(RenderBackend::GetInstance().GetDevice(), frame.CommandQueueFence, nullptr);
			vkDestroySemaphore(RenderBackend::GetInstance().GetDevice(), frame.imageAvailableSemaphore, nullptr);
			vkDestroySemaphore(RenderBackend::GetInstance().GetDevice(), frame.renderFinishedSemaphore, nullptr);
		}
		m_virtualFrames.clear();
	}
	
	VirtualFrame& VirtualFrameProvider::GetCurrentFrame()
	{
		return m_virtualFrames[m_currentFrame];
	}

	VirtualFrame& VirtualFrameProvider::GetNextFrame()
	{
		return m_virtualFrames[(m_currentFrame + 1) % m_virtualFrames.size()];
	}

	const VirtualFrame& VirtualFrameProvider::GetCurrentFrame() const
	{
		return m_virtualFrames[m_currentFrame];
	}

	const VirtualFrame& VirtualFrameProvider::GetNextFrame() const
	{
		return m_virtualFrames[(m_currentFrame + 1) % m_virtualFrames.size()];
	}

	uint32_t VirtualFrameProvider::GetPresentImageIndex() const
	{
		return m_presentImageIndex;
	}

	size_t VirtualFrameProvider::GetFrameCount() const
	{
		return m_virtualFrames.size();
	}

	void VirtualFrameProvider::StartFrame()
	{
		auto& frame = GetCurrentFrame();
		auto& vulkanContext = RenderBackend::GetInstance();
		vkWaitForFences(RenderBackend::GetInstance().GetDevice(), 1, &(frame.CommandQueueFence), VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(
			RenderBackend::GetInstance().GetDevice(), 
			RenderBackend::GetInstance().GetSwapChain(), 
			UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &m_presentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RenderBackend::GetInstance().RecreateSwapChain();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		vkResetFences(RenderBackend::GetInstance().GetDevice(), 1, &(frame.CommandQueueFence));

		frame.Commands.Begin();
	}

	void VirtualFrameProvider::EndFrame()
	{
		auto& frame = this->GetCurrentFrame();
		auto& backend = RenderBackend::GetInstance();

		frame.Commands.End();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &(frame.Commands.GetCommandBufferHandle());
		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(RenderBackend::GetInstance().GetGraphicsQueue(), 1, &submitInfo, frame.CommandQueueFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		// 很大概率是信号量的控制有问题；要不就是RenderPass绘制出问题了；
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.pResults = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &(RenderBackend::GetInstance().GetSwapChain());
		presentInfo.pImageIndices = &m_presentImageIndex;
		VkResult result = vkQueuePresentKHR(RenderBackend::GetInstance().GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RenderBackend::GetInstance().RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_currentFrame = (m_currentFrame + 1) % m_virtualFrames.size();
	}
}