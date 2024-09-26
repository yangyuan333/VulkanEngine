#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	struct TextureDesc {
		VkFormat format;
		VkImageUsageFlags usage;
		VkMemoryPropertyFlags memoryUsage;
		VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct BufferDesc {
		size_t byteSize;
		VkBufferUsageFlags usage;
		VkMemoryPropertyFlags memoryUsage;
	};

	struct ClearColor {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;
	};

	struct ClearDepthStencil {
		float depth = 1.0f;
		uint32_t stencil = 0;
	};
}