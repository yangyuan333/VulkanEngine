#include "Image.h"
#include "../Render/RenderBackend.h"
#include "../Utility/UtilityFunc.h"
#include <cmath>

namespace VulkanEngine
{
	uint32_t ComputeMipmaplevels(uint32_t width, uint32_t height)
	{
		return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
	}

	uint32_t CalculateImageLayerCount(ImageOptions::Value options) {
		if (options & ImageOptions::CUBEMAP) return 6;
		else return 1;
	}

	VkImageSubresourceRange GetDefaultImageSubresourceRange(const Image& image) {
		return VkImageSubresourceRange{ ImageFormatToImageAspect(image.GetFormat()),
										 0, // base mip level
										 image.GetMipLevelCount(),
										 0, // base layer
										 image.GetLayerCount() };
	}

	Image::Image(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options)
		: m_extent{width, height}, m_format(format), m_usage(usage), m_memoryUsage(memoryUsage), m_options(options)
	{
		Init(width,height,format,usage,memoryUsage,options);
	}

    Image::Image(VkImage image, uint32_t width, uint32_t height, VkFormat format)
    {
		m_image = image;
		m_extent = { width , height};
		m_format = format;

		InitViews(m_image, format);
		m_currentMipLayout.resize(m_mipLevelCount, VK_IMAGE_LAYOUT_UNDEFINED);
    }

	Image::Image(Image&& other)
	{
		m_image = other.m_image;
		m_imageView = other.m_imageView;
		m_imageMemory = other.m_imageMemory;
		m_extent = other.m_extent;
		m_mipLevelCount = other.m_mipLevelCount;
		m_layerCount = other.m_layerCount;
		m_format = other.m_format;
		m_usage = other.m_usage;
		m_memoryUsage = other.m_memoryUsage;
		m_options = other.m_options;
		m_currentMipLayout = other.m_currentMipLayout;

		other.m_image = VK_NULL_HANDLE;
		other.m_imageView = VK_NULL_HANDLE;
		other.m_imageMemory = VK_NULL_HANDLE;
	}

	Image& Image::operator=(Image&& other)
	{
		// TODO: 在此处插入 return 语句
		Destroy();

		m_image = other.m_image;
		m_imageView = other.m_imageView;
		m_imageMemory = other.m_imageMemory;
		m_extent = other.m_extent;
		m_mipLevelCount = other.m_mipLevelCount;
		m_layerCount = other.m_layerCount;
		m_format = other.m_format;
		m_usage = other.m_usage;
		m_memoryUsage = other.m_memoryUsage;
		m_options = other.m_options;
		m_currentMipLayout = other.m_currentMipLayout;

		other.m_image = VK_NULL_HANDLE;
		other.m_imageView = VK_NULL_HANDLE;
		other.m_imageMemory = VK_NULL_HANDLE;

		return *this;
	}

	Image::~Image()
	{
		Destroy();
	}

	void Image::Init(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options)
	{
		m_extent = VkExtent2D{ width , height };
		m_format = format;
		m_usage = usage;
		m_memoryUsage = memoryUsage;
		m_options = options;

		m_mipLevelCount = 1;
		if (options & ImageOptions::MIPMAPS)
		{
			m_mipLevelCount = ComputeMipmaplevels(width, height);
		}
		m_layerCount = CalculateImageLayerCount(options);
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = m_mipLevelCount;
		imageInfo.arrayLayers = m_layerCount;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = RenderBackend::GetInstance().GetMsaaSampleBit();
		imageInfo.flags = 0;
		if (options & ImageOptions::CUBEMAP)
			imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		if (vkCreateImage(RenderBackend::GetInstance().GetDevice(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(RenderBackend::GetInstance().GetDevice(), m_image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
			memRequirements.memoryTypeBits, memoryUsage);
		if (vkAllocateMemory(RenderBackend::GetInstance().GetDevice(), &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(RenderBackend::GetInstance().GetDevice(), m_image, m_imageMemory, 0);

		InitViews(m_image, format);

		m_currentMipLayout.resize(m_mipLevelCount, VK_IMAGE_LAYOUT_UNDEFINED);
	}
	
	VkImageView Image::GetImageView() const
	{
		return m_imageView;
	}

	uint32_t Image::GetMipLevelWidth(uint32_t mipLevel) const
	{
		auto safeWidth = std::max(GetWidth(), 1u << mipLevel);
		return safeWidth >> mipLevel;
	}

	uint32_t Image::GetMipLevelHeight(uint32_t mipLevel) const
	{
		auto safeHeight = std::max(GetHeight(), 1u << mipLevel);
		return safeHeight >> mipLevel;
	}

	void Image::Destroy()
	{
		if (m_image != VK_NULL_HANDLE)
		{
			vkDestroyImageView(RenderBackend::GetInstance().GetDevice(), m_imageView, nullptr);
			vkDestroyImage(RenderBackend::GetInstance().GetDevice(), m_image, nullptr);
			vkFreeMemory(RenderBackend::GetInstance().GetDevice(), m_imageMemory, nullptr);
			m_imageView = VK_NULL_HANDLE;
			m_image = VK_NULL_HANDLE;
			m_imageMemory = VK_NULL_HANDLE;
		}
	}

	void Image::InitViews(const VkImage& image, VkFormat format)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;

		if (m_options & ImageOptions::CUBEMAP)
		{
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		}

		VkImageSubresourceRange subresourceRange = GetDefaultImageSubresourceRange(*this);
		viewInfo.subresourceRange = subresourceRange;
		if (vkCreateImageView(RenderBackend::GetInstance().GetDevice(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}
}