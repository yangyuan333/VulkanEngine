#pragma once
#include <utility>
#include <vulkan/vulkan.h>
#include "../Render/RenderResource.h"

namespace VulkanEngine
{
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	bool hasStencilComponent(VkFormat format);
	StageMask decideTransPipelineBarrierStageMask(VkImageLayout oldLayout, VkImageLayout newLayout);
	VkImageLayout ImageUsageToImageLayout(VkImageUsageFlagBits usage);
	VkAccessFlags BufferUsageToStage(VkBufferUsageFlags usage);
	VkImageAspectFlags ImageFormatToImageAspect(VkFormat format);
}