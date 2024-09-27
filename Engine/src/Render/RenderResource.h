#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <memory>
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

	struct ShaderImageDesc {
		std::shared_ptr<Image> image;
		VkImageUsageFlags usage;
		std::shared_ptr<Sampler> sampler;
	};

	struct ShaderBufferDesc {
		std::shared_ptr<Buffer> buffer;
		size_t offset;
		size_t range;
	};

	struct ShadowConfig
	{
		VkBool32 depthClampEnable;
		VkBool32 depthBiasEnable;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
	};

	struct CullConfig
	{
		VkCullModeFlags cullMode;
		VkFrontFace frontFace;
	};

	struct DepthStencilConfig
	{
		VkBool32 depthTestEnable;
		VkBool32 depthWriteEnable;
		VkCompareOp depthCompareOp;
		VkBool32 stencilTestEnable;
		VkStencilOpState stencilFront;
		VkStencilOpState stencilBack;
	};

	struct ColorAttachmentBlendConfig
	{
		VkBool32 blendEnable;
		VkBlendOp colorBlendOp;
		VkBlendFactor srcColorBlendFactor;
		VkBlendFactor dstColorBlendFactor;
		VkBlendOp alphaBlendOp;
		VkBlendFactor srcAlphaBlendFactor;
		VkBlendFactor dstAlphaBlendFactor;
	};

	struct ColorBlendConfig
	{
		std::vector<ColorAttachmentBlendConfig> colorAttachmentConfig;
	};

	struct PipelineConfig
	{
		std::vector<VkDynamicState> dynamicStates;
		VkVertexInputBindingDescription vertexBindingDescription;
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		VkPrimitiveTopology primitiveTopology;
		CullConfig cullConfig;
		ShadowConfig shadowConfig;
		DepthStencilConfig depthStencilConifg;
		ColorBlendConfig colorBlendConfig;
	};
	
}