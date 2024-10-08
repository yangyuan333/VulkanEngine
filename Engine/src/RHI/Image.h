#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "../Resource/ImageLoader.h"

namespace VulkanEngine
{
	struct ImageOptions {
		using Value = uint32_t;
		enum Bits : Value {
			DEFAULT = 0,
			MIPMAPS = 1 << 0,
			CUBEMAP = 1 << 1,
		};
	};

	// 2D image And Cube image---Image2D array之后再拓展
	// 还得继续封装，能让对image数据进行赋值
	class Image
	{
	public:
		Image() = default;
		Image(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options);
		Image(VkImage image, uint32_t width, uint32_t height, VkFormat format);
		Image(Image&& other);

		Image& operator=(Image&& other);
		Image(Image const& other) = delete;
		Image& operator=(Image const& other) = delete;
		~Image();

		void Init(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options);

		VkImageView GetImageView() const;
		uint32_t GetMipLevelWidth(uint32_t mipLevel) const;
		uint32_t GetMipLevelHeight(uint32_t mipLevel) const;

		auto GetImageHandle() const { return m_image; }
		auto GetFormat() const { return m_format; }
		auto GetWidth() const { return m_extent.width; }
		auto GetHeight() const { return m_extent.height; }
		auto GetMipLevelCount() const { return m_mipLevelCount; }
		auto GetLayerCount() const { return m_layerCount; }
		auto GetImageUsage() const { return m_usage; }
		auto GetImagememoryUsage() const { return m_memoryUsage; }
		auto GetImageMipLayout(uint32_t mipLevel) const { return m_currentMipLayout[mipLevel]; }
		void SetImageMipLayout(VkImageLayout layout, uint32_t mipLevel = 0) const { m_currentMipLayout[mipLevel] = layout; }
	
	private:
		void Destroy();
		void InitViews(const VkImage& image, VkFormat format);

	private:
		VkImage m_image;
		VkDeviceMemory m_imageMemory;
		VkImageView m_imageView;

		VkExtent2D m_extent{ 0u, 0u };
		uint32_t m_mipLevelCount{ 1 };
		uint32_t m_layerCount{ 1 };
		VkFormat m_format{ VkFormat::VK_FORMAT_UNDEFINED };
		VkImageUsageFlags m_usage;
		VkMemoryPropertyFlags m_memoryUsage;
		ImageOptions::Value m_options;
		
		mutable std::vector<VkImageLayout> m_currentMipLayout;
	};
}