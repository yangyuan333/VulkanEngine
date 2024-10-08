#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <memory>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "../RHI/Image.h"
#include "../RHI/Sampler.h"
#include "../RHI/Buffer.h"

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

	struct ImageInfo {
		std::reference_wrapper<const Image> refImage;
		VkImageUsageFlagBits usage;
		uint32_t mipLevel = 0;
		uint32_t layer = 0;
	};

	struct BufferInfo {
		std::reference_wrapper<const Buffer> refBuffer;
		uint32_t offset = 0;
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

	struct Rect2D {
		int32_t  offsetWidth = 0;
		int32_t  offsetHeight = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct Viewport {
		float offsetWidth = 0.0f;
		float offsetHeight = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;
	};

	struct StageMask
	{
		VkAccessFlags srcMask;
		VkAccessFlags desMask;
		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags desStage;
	};

	struct Vertex
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec2 TexCoord{ 0.0f, 0.0f };
		glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Tangent{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Bitangent{ 0.0f, 0.0f, 0.0f };

		static VkVertexInputBindingDescription GetInputBindingDescription();

		static std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions();
	};
}