#pragma once

#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	class Image
	{
	private:
		VkImage m_image;
		VkImageView m_imageView;
		VkDeviceMemory m_imageMemory;
	};
}