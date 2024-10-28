#include "UtilityFunc.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	static VkFormat FormatTable[] = {
		VkFormat::VK_FORMAT_UNDEFINED,
		VkFormat::VK_FORMAT_R4G4_UNORM_PACK8,
		VkFormat::VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		VkFormat::VK_FORMAT_B4G4R4A4_UNORM_PACK16,
		VkFormat::VK_FORMAT_R5G6B5_UNORM_PACK16,
		VkFormat::VK_FORMAT_B5G6R5_UNORM_PACK16,
		VkFormat::VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		VkFormat::VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		VkFormat::VK_FORMAT_A1R5G5B5_UNORM_PACK16,
		VkFormat::VK_FORMAT_R8_UNORM,
		VkFormat::VK_FORMAT_R8_SNORM,
		VkFormat::VK_FORMAT_R8_USCALED,
		VkFormat::VK_FORMAT_R8_SSCALED,
		VkFormat::VK_FORMAT_R8_UINT,
		VkFormat::VK_FORMAT_R8_SINT,
		VkFormat::VK_FORMAT_R8_SRGB,
		VkFormat::VK_FORMAT_R8G8_UNORM,
		VkFormat::VK_FORMAT_R8G8_SNORM,
		VkFormat::VK_FORMAT_R8G8_USCALED,
		VkFormat::VK_FORMAT_R8G8_SSCALED,
		VkFormat::VK_FORMAT_R8G8_UINT,
		VkFormat::VK_FORMAT_R8G8_SINT,
		VkFormat::VK_FORMAT_R8G8_SRGB,
		VkFormat::VK_FORMAT_R8G8B8_UNORM,
		VkFormat::VK_FORMAT_R8G8B8_SNORM,
		VkFormat::VK_FORMAT_R8G8B8_USCALED,
		VkFormat::VK_FORMAT_R8G8B8_SSCALED,
		VkFormat::VK_FORMAT_R8G8B8_UINT,
		VkFormat::VK_FORMAT_R8G8B8_SINT,
		VkFormat::VK_FORMAT_R8G8B8_SRGB,
		VkFormat::VK_FORMAT_B8G8R8_UNORM,
		VkFormat::VK_FORMAT_B8G8R8_SNORM,
		VkFormat::VK_FORMAT_B8G8R8_USCALED,
		VkFormat::VK_FORMAT_B8G8R8_SSCALED,
		VkFormat::VK_FORMAT_B8G8R8_UINT,
		VkFormat::VK_FORMAT_B8G8R8_SINT,
		VkFormat::VK_FORMAT_B8G8R8_SRGB,
		VkFormat::VK_FORMAT_R8G8B8A8_UNORM,
		VkFormat::VK_FORMAT_R8G8B8A8_SNORM,
		VkFormat::VK_FORMAT_R8G8B8A8_USCALED,
		VkFormat::VK_FORMAT_R8G8B8A8_SSCALED,
		VkFormat::VK_FORMAT_R8G8B8A8_UINT,
		VkFormat::VK_FORMAT_R8G8B8A8_SINT,
		VkFormat::VK_FORMAT_R8G8B8A8_SRGB,
		VkFormat::VK_FORMAT_B8G8R8A8_UNORM,
		VkFormat::VK_FORMAT_B8G8R8A8_SNORM,
		VkFormat::VK_FORMAT_B8G8R8A8_USCALED,
		VkFormat::VK_FORMAT_B8G8R8A8_SSCALED,
		VkFormat::VK_FORMAT_B8G8R8A8_UINT,
		VkFormat::VK_FORMAT_B8G8R8A8_SINT,
		VkFormat::VK_FORMAT_B8G8R8A8_SRGB,
		VkFormat::VK_FORMAT_A8B8G8R8_UNORM_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_SNORM_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_UINT_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_SINT_PACK32,
		VkFormat::VK_FORMAT_A8B8G8R8_SRGB_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_UINT_PACK32,
		VkFormat::VK_FORMAT_A2R10G10B10_SINT_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_UINT_PACK32,
		VkFormat::VK_FORMAT_A2B10G10R10_SINT_PACK32,
		VkFormat::VK_FORMAT_R16_UNORM,
		VkFormat::VK_FORMAT_R16_SNORM,
		VkFormat::VK_FORMAT_R16_USCALED,
		VkFormat::VK_FORMAT_R16_SSCALED,
		VkFormat::VK_FORMAT_R16_UINT,
		VkFormat::VK_FORMAT_R16_SINT,
		VkFormat::VK_FORMAT_R16_SFLOAT,
		VkFormat::VK_FORMAT_R16G16_UNORM,
		VkFormat::VK_FORMAT_R16G16_SNORM,
		VkFormat::VK_FORMAT_R16G16_USCALED,
		VkFormat::VK_FORMAT_R16G16_SSCALED,
		VkFormat::VK_FORMAT_R16G16_UINT,
		VkFormat::VK_FORMAT_R16G16_SINT,
		VkFormat::VK_FORMAT_R16G16_SFLOAT,
		VkFormat::VK_FORMAT_R16G16B16_UNORM,
		VkFormat::VK_FORMAT_R16G16B16_SNORM,
		VkFormat::VK_FORMAT_R16G16B16_USCALED,
		VkFormat::VK_FORMAT_R16G16B16_SSCALED,
		VkFormat::VK_FORMAT_R16G16B16_UINT,
		VkFormat::VK_FORMAT_R16G16B16_SINT,
		VkFormat::VK_FORMAT_R16G16B16_SFLOAT,
		VkFormat::VK_FORMAT_R16G16B16A16_UNORM,
		VkFormat::VK_FORMAT_R16G16B16A16_SNORM,
		VkFormat::VK_FORMAT_R16G16B16A16_USCALED,
		VkFormat::VK_FORMAT_R16G16B16A16_SSCALED,
		VkFormat::VK_FORMAT_R16G16B16A16_UINT,
		VkFormat::VK_FORMAT_R16G16B16A16_SINT,
		VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
		VkFormat::VK_FORMAT_R32_UINT,
		VkFormat::VK_FORMAT_R32_SINT,
		VkFormat::VK_FORMAT_R32_SFLOAT,
		VkFormat::VK_FORMAT_R32G32_UINT,
		VkFormat::VK_FORMAT_R32G32_SINT,
		VkFormat::VK_FORMAT_R32G32_SFLOAT,
		VkFormat::VK_FORMAT_R32G32B32_UINT,
		VkFormat::VK_FORMAT_R32G32B32_SINT,
		VkFormat::VK_FORMAT_R32G32B32_SFLOAT,
		VkFormat::VK_FORMAT_R32G32B32A32_UINT,
		VkFormat::VK_FORMAT_R32G32B32A32_SINT,
		VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
		VkFormat::VK_FORMAT_R64_UINT,
		VkFormat::VK_FORMAT_R64_SINT,
		VkFormat::VK_FORMAT_R64_SFLOAT,
		VkFormat::VK_FORMAT_R64G64_UINT,
		VkFormat::VK_FORMAT_R64G64_SINT,
		VkFormat::VK_FORMAT_R64G64_SFLOAT,
		VkFormat::VK_FORMAT_R64G64B64_UINT,
		VkFormat::VK_FORMAT_R64G64B64_SINT,
		VkFormat::VK_FORMAT_R64G64B64_SFLOAT,
		VkFormat::VK_FORMAT_R64G64B64A64_UINT,
		VkFormat::VK_FORMAT_R64G64B64A64_SINT,
		VkFormat::VK_FORMAT_R64G64B64A64_SFLOAT,
		VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32,
		VkFormat::VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
		VkFormat::VK_FORMAT_D16_UNORM,
		VkFormat::VK_FORMAT_X8_D24_UNORM_PACK32,
		VkFormat::VK_FORMAT_D32_SFLOAT,
		VkFormat::VK_FORMAT_S8_UINT,
		VkFormat::VK_FORMAT_D16_UNORM_S8_UINT,
		VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,
		VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,
	};

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
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			stageMask.srcMask = VK_ACCESS_TRANSFER_READ_BIT;
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
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			stageMask.srcMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			stageMask.desMask = VK_ACCESS_TRANSFER_READ_BIT;
			stageMask.srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			stageMask.desStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
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

    void FillImage(Image& image, const ImageData& imageData, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryUsage, ImageOptions::Value options)
    {
		auto& backend = RenderBackend::GetInstance();
		auto commandBuffer = backend.BeginSingleTimeCommand();
		auto& stageBuffer = RenderBackend::GetInstance().GetStagingBuffer();
		image.Init(imageData.Width, imageData.Height, FormatTable[int(imageData.ImageFormat)], usage, memoryUsage, options);
		commandBuffer->TransferLayout(image, image.GetImageMipLayout(0), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, image.GetMipLevelCount(), 0, 1);
		
		// imagedata --> stagingbuffer --> image

		auto allocation = stageBuffer.Submit(imageData.ByteData.data(), imageData.ByteData.size() * sizeof(uint8_t));
		stageBuffer.Flush();
		commandBuffer->CopyBufferToImage(
			stageBuffer.GetBuffer(), allocation.Offset,
			image, 0, 0);

		if (options & ImageOptions::MIPMAPS)
		{
			commandBuffer->GenerateImageMipmap(image, VkFilter::VK_FILTER_LINEAR);
		}
		else
		{
			commandBuffer->TransferLayout(image, image.GetImageMipLayout(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		RenderBackend::GetInstance().SubmitSingleTimeCommand(commandBuffer->GetCommandBufferHandle());

		stageBuffer.Reset();
	}
}