#include "UtilityFunc.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(RenderBackend::GetInstance().GetPhyDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	bool hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}


	VkAccessFlags BufferUsageToStage(VkBufferUsageFlags usage)
	{
		VkAccessFlags flag;
		if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		{
			flag |= VK_ACCESS_TRANSFER_READ_BIT;	
		}

		if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			flag |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			flag |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
		{
			flag |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		}

		if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
		{
			flag |= VK_ACCESS_TRANSFER_READ_BIT;
		}

		if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		{
			flag |= VK_ACCESS_TRANSFER_READ_BIT;
		}

		return flag;
	}

	StageMask decideTransPipelineBarrierStageMask(
		VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		StageMask stageMask;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			stageMask.srcMask = 0;
			stageMask.desMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			stageMask.srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			stageMask.desStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			stageMask.srcMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			stageMask.desMask = VK_ACCESS_SHADER_READ_BIT;
			stageMask.srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			stageMask.desStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			stageMask.srcMask = 0;
			stageMask.desMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;;
			stageMask.srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			stageMask.desStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		return stageMask;
	}

	VkImageAspectFlags ImageFormatToImageAspect(VkFormat format) {
		switch (format) {
		case VkFormat::VK_FORMAT_D16_UNORM:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		case VkFormat::VK_FORMAT_X8_D24_UNORM_PACK32:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		case VkFormat::VK_FORMAT_D32_SFLOAT:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		default:
			return VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}
}