#pragma once

#include "CommandBuffer.h"
#include <vector>

namespace VulkanEngine
{
	struct VirtualFrame {
		CommandBuffer Commands{ VkCommandBuffer{} };
		VkFence CommandQueueFence;
		VkSemaphore imageAvailableSemaphores;
		VkSemaphore renderFinishedSemaphores;
	};

	class VirtualFrameProvider {
	public:
		void Init(size_t frameCount);
		void Destroy();

		void StartFrame();
		VirtualFrame& GetCurrentFrame();
		VirtualFrame& GetNextFrame();
		[[nodiscard]] const VirtualFrame& GetCurrentFrame() const;
		[[nodiscard]] const VirtualFrame& GetNextFrame() const;
		[[nodiscard]] uint32_t GetPresentImageIndex() const;
		[[nodiscard]] size_t GetFrameCount() const;
		void EndFrame();

	private:
		std::vector<VirtualFrame> m_virtualFrames;
		uint32_t m_presentImageIndex = 0;
		size_t m_currentFrame = 0;
	};
}