#pragma once

#include "CommandBuffer.h"
#include "../Render/StagingBuffer.h"
#include <vector>

namespace VulkanEngine
{
	struct VirtualFrame {
		CommandBuffer Commands{ VkCommandBuffer{} };
		VkFence CommandQueueFence;
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		StagingBuffer stagingBuffer;
	};

	class VirtualFrameProvider {
	public:
		void Init(size_t frameCount, size_t stagingBufferSize);
		void Destroy();

		void StartFrame();
		VirtualFrame& GetCurrentFrame();
		VirtualFrame& GetNextFrame();
		const VirtualFrame& GetCurrentFrame() const;
		const VirtualFrame& GetNextFrame() const;
		uint32_t GetPresentImageIndex() const;
		size_t GetFrameCount() const;
		void EndFrame();

	private:
		std::vector<VirtualFrame> m_virtualFrames;
		uint32_t m_presentImageIndex = 0;
		size_t m_currentFrame = 0;
	};
}