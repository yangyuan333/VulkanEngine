#include "CommandBuffer.h"
#include "../Render/RenderBackend.h"
#include "../Utility/UtilityFunc.h"

#include <stdexcept>

namespace VulkanEngine
{
	void CommandBuffer::BeginRenderPass(RenderPass const& renderPass)
	{
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.GetRenderPassHandle();
		if (renderPass.GetFameBuffer()->GetFrameBufferHandle() == nullptr)
			throw std::runtime_error("renderpass not bind framebuffer!");
		renderPassInfo.framebuffer = renderPass.GetFameBuffer()->GetFrameBufferHandle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = {renderPass.GetFameBuffer()->GetWidth(), renderPass.GetFameBuffer()->GetHeight() };
		std::vector<VkClearValue> clearColor = renderPass.GetClearColor();
		renderPassInfo.clearValueCount = clearColor.size();
		renderPassInfo.pClearValues = clearColor.data();
		vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_commandBuffer);
	}
	void CommandBuffer::BindDescriptorSet(VkPipelineLayout layout, std::vector<VkDescriptorSet>& descriptorSets, uint32_t firstIndex, VkPipelineBindPoint bindPoint)
	{
		vkCmdBindDescriptorSets(
			m_commandBuffer, bindPoint,
			layout, firstIndex, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
	}
	void CommandBuffer::BindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint)
	{
		vkCmdBindPipeline(m_commandBuffer, bindPoint, pipeline.GetPipelineHandle());
	}
	void CommandBuffer::BindVertexBuffer(const Buffer& buffer, uint32_t binding, VkDeviceSize offset)
	{
		VkBuffer vertexBuffers[] = { buffer.GetBufferHandle() };
		vkCmdBindVertexBuffers(m_commandBuffer, binding, 1, vertexBuffers, &offset);
	}
	void CommandBuffer::BindVertexBuffers(std::vector<std::reference_wrapper<const Buffer>> const& buffers, uint32_t binding, std::vector<VkDeviceSize> const& offset)
	{
		std::vector<VkBuffer> vertexBuffers; vertexBuffers.resize(buffers.size());
		int index = 0;
		for (auto const& buffer : buffers) vertexBuffers[index++] = buffer.get().GetBufferHandle();
		std::vector<VkDeviceSize> offsets = offset;
		if (offset.size() != buffers.size()) offsets.resize(buffers.size(), 0);
		vkCmdBindVertexBuffers(m_commandBuffer, binding, vertexBuffers.size(), vertexBuffers.data(), offsets.data());
	}
	void CommandBuffer::BindIndexBufferUInt32(const Buffer& indexBuffer, VkDeviceSize offset)
	{
		vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer.GetBufferHandle(), offset, VK_INDEX_TYPE_UINT32);
	}
	void CommandBuffer::BindIndexBufferUInt16(const Buffer& indexBuffer, VkDeviceSize offset)
	{
		vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer.GetBufferHandle(), offset, VK_INDEX_TYPE_UINT16);
	}
	void CommandBuffer::Destroy()
	{
		if (m_commandBuffer != VK_NULL_HANDLE)
			vkFreeCommandBuffers(RenderBackend::GetInstance().GetDevice(), RenderBackend::GetInstance().GetCommandPool(), 1, &m_commandBuffer);
		m_commandBuffer = VK_NULL_HANDLE;
	}
	CommandBuffer::CommandBuffer(CommandFlag commandFlag)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		switch (commandFlag)
		{
			case CommandFlag::Primary:
			{
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				break;
			}
			case CommandFlag::Secondary:
			{
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				break;
			}
		}
		allocInfo.commandPool = RenderBackend::GetInstance().GetCommandPool();
		allocInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(RenderBackend::GetInstance().GetDevice(), &allocInfo, &m_commandBuffer);
	}
	CommandBuffer::~CommandBuffer()
	{
		Destroy();
	}
	void CommandBuffer::Begin(CommandBeginFlag commandFlag)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		switch (commandFlag)
		{
			case CommandBeginFlag::Default:
			{
				beginInfo.flags = 0;
				break;
			}
			case CommandBeginFlag::OneTime:
			{
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				break;
			}
		}
		beginInfo.pInheritanceInfo = nullptr; // 这个先不管，V2.0的时候再把 second command buffer 用上;
		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
	void CommandBuffer::End()
	{
		if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	void CommandBuffer::SubmitImmediate(VkQueue queue)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);
		// vkFreeCommandBuffers(RenderBackend::GetInstance().GetDevice(), RenderBackend::GetInstance().GetCommandPool(), 1, &m_commandBuffer);
	}
	void CommandBuffer::EndAndSubmitImmediate(VkQueue queue)
	{
		End();
		SubmitImmediate(queue);
	}
	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount)
	{
		vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, 0, 0);
	}
	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}
	void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount)
	{
		vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, 0, 0, 0);
	}
	void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
	{
		vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}
	void CommandBuffer::SetViewport(const Viewport& viewport)
	{
		VkViewport vkviewport{};
		vkviewport.x = viewport.offsetWidth;
		vkviewport.y = viewport.offsetHeight;
		vkviewport.width = viewport.width;
		vkviewport.height = viewport.height;
		vkviewport.minDepth = viewport.minDepth;
		vkviewport.maxDepth = viewport.maxDepth;
		vkCmdSetViewport(m_commandBuffer, 0, 1, &vkviewport);
	}
	void CommandBuffer::SetScissor(const Rect2D& scissor)
	{
		VkRect2D vkscissor{};
		vkscissor.offset = { scissor.offsetWidth, scissor.offsetHeight };
		vkscissor.extent = { scissor.width, scissor.height };
		vkCmdSetScissor(m_commandBuffer, 0, 1, &vkscissor);
	}
	void CommandBuffer::CopyImage(VkQueue queue, const Image& source, uint32_t src_mipLevel, uint32_t src_layer, const Image& destination, uint32_t des_mipLevel, uint32_t des_layer)
	{
		TransferLayout(queue, source, source.GetImageMipLayout(src_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src_mipLevel, 1, src_layer, 1);
		TransferLayout(queue, destination, destination.GetImageMipLayout(des_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, des_mipLevel, 1, des_layer, 1);

		VkImageCopy copyRegion;
		copyRegion.srcOffset = {0, 0, 0};
		copyRegion.dstOffset = {0, 0, 0};
		copyRegion.srcSubresource = { ImageFormatToImageAspect(source.GetFormat()), src_mipLevel, src_layer, 1};
		copyRegion.dstSubresource = { ImageFormatToImageAspect(destination.GetFormat()), des_mipLevel, des_layer, 1};
		copyRegion.extent = { source.GetMipLevelWidth(src_mipLevel), source.GetMipLevelHeight(src_mipLevel), 1};

		vkCmdCopyImage(
			m_commandBuffer,
			source.GetImageHandle(),
			source.GetImageMipLayout(src_mipLevel),
			destination.GetImageHandle(),
			destination.GetImageMipLayout(des_mipLevel),
			1, &copyRegion);
	}
	void CommandBuffer::CopyBufferToImage(VkQueue queue, const Buffer& source, uint32_t offset, const Image& destination, uint32_t des_mipLevel, uint32_t des_layer)
	{
		TransferLayout(queue, destination, destination.GetImageMipLayout(des_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, des_mipLevel, 1, des_layer, 1);

		VkBufferImageCopy region;
		region.bufferOffset = offset;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = ImageFormatToImageAspect(destination.GetFormat());
		region.imageSubresource.mipLevel = des_mipLevel;
		region.imageSubresource.baseArrayLayer = des_layer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { destination.GetMipLevelWidth(des_mipLevel), destination.GetMipLevelHeight(des_mipLevel), 1};

		vkCmdCopyBufferToImage(
			m_commandBuffer,
			source.GetBufferHandle(),
			destination.GetImageHandle(),
			destination.GetImageMipLayout(des_mipLevel),
			1, &region);
	}
	void CommandBuffer::CopyImageToBuffer(VkQueue queue, const Image& source, uint32_t src_mipLevel, uint32_t src_layer, const Buffer& destination, uint32_t offset)
	{
		TransferLayout(queue, source, source.GetImageMipLayout(src_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src_mipLevel, 1, src_layer, 1);

		VkBufferImageCopy region;
		region.bufferOffset = offset;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = ImageFormatToImageAspect(source.GetFormat());
		region.imageSubresource.mipLevel = src_mipLevel;
		region.imageSubresource.baseArrayLayer = src_layer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { source.GetMipLevelWidth(src_mipLevel), source.GetMipLevelHeight(src_mipLevel), 1 };

		vkCmdCopyImageToBuffer(
			m_commandBuffer,
			source.GetImageHandle(),
			source.GetImageMipLayout(src_mipLevel),
			destination.GetBufferHandle(),
			1, &region);

		// 这里没法统一设置，得看这个buffer会用作啥
		// VkBufferMemoryBarrier barrier;
		// barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		// barrier.pNext = nullptr;
		// barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		// barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		// barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.buffer = destination.GetBufferHandle();
		// barrier.offset = des_offset;
		// barrier.size = byteSize;
		// vkCmdPipelineBarrier(
		// 	m_commandBuffer,
		// 	VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT | VK_ACCESS_SHADER_READ_BIT, // 这里要好好考虑一下
		// 	0,
		// 	0, nullptr,
		// 	1, &barrier,
		// 	0, nullptr);
	}
	void CommandBuffer::CopyBuffer(VkQueue queue, const Buffer& source, uint32_t src_offset, const Buffer& destination, uint32_t des_offset, size_t byteSize)
	{
		VkBufferCopy region;
		region.srcOffset = src_offset;
		region.dstOffset = des_offset;
		region.size = byteSize;
		vkCmdCopyBuffer(
			m_commandBuffer,
			source.GetBufferHandle(), destination.GetBufferHandle(),
			1, &region);

		// 这里没法统一设置，得看这个buffer会用作啥
		// VkBufferMemoryBarrier barrier;
		// barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		// barrier.pNext = nullptr;
		// barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		// barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		// barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.buffer = destination.GetBufferHandle();
		// barrier.offset = des_offset;
		// barrier.size = byteSize;
		// vkCmdPipelineBarrier(
		// 	m_commandBuffer,
		// 	VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT | VK_ACCESS_SHADER_READ_BIT, // 这里要好好考虑一下
		// 	0,
		// 	0, nullptr,
		// 	1, &barrier,
		// 	0, nullptr);
	}
	void CommandBuffer::BlitImage(
		VkQueue queue, const Image& source, const Image& destination, VkFilter filter,
		uint32_t src_mipLevel, uint32_t src_firstArray, uint32_t src_arrayCount,
		uint32_t des_mipLevel, uint32_t des_firstArray, uint32_t des_arrayCount)
	{
		
		TransferLayout(queue, source, source.GetImageMipLayout(src_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src_mipLevel, 1, src_firstArray, src_arrayCount);
		TransferLayout(queue, destination, destination.GetImageMipLayout(des_mipLevel), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, des_mipLevel, 1, des_firstArray, des_arrayCount);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { (int32_t)source.GetMipLevelWidth(src_mipLevel), (int32_t)source.GetMipLevelHeight(src_mipLevel), 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = src_mipLevel;
		blit.srcSubresource.baseArrayLayer = src_firstArray;
		blit.srcSubresource.layerCount = src_arrayCount;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { (int32_t)destination.GetMipLevelWidth(des_mipLevel), (int32_t)destination.GetMipLevelHeight(des_mipLevel), 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = des_mipLevel;
		blit.dstSubresource.baseArrayLayer = des_firstArray;
		blit.dstSubresource.layerCount = des_arrayCount;

		vkCmdBlitImage(
			m_commandBuffer,
			source.GetImageHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			destination.GetImageHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			filter);
	}
	void CommandBuffer::GenerateImageMipmap(VkQueue queue, const Image& image, VkImageUsageFlags initialUsage, VkFilter filter)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(RenderBackend::GetInstance().GetPhyDevice(), image.GetFormat(), &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		TransferLayout(queue, image, image.GetImageMipLayout(0), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, image.GetMipLevelCount(), 0, 1);

		int32_t mipWidth = image.GetWidth();
		int32_t mipHeight = image.GetHeight();
		for (uint32_t i = 1; i < image.GetMipLevelCount(); ++i)
		{
			BlitImage(
				queue, image, image, VK_FILTER_LINEAR,
				i - 1, 0, 1,
				i, 0, 1);
		}
	}
	void CommandBuffer::TransferLayout(VkQueue queue, const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t firstMip, uint32_t mipLevels, uint32_t firstArray, uint32_t arrayCount)
	{
		if (mipLevels > 1)
			for (int i = 0; i < mipLevels; ++i)
				if (oldLayout != image.GetImageMipLayout(i + firstMip))  throw std::runtime_error("image mipmap layout is not same!");

		if (oldLayout == newLayout) return;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image.GetImageHandle();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		barrier.subresourceRange.aspectMask = ImageFormatToImageAspect(image.GetFormat());

		barrier.subresourceRange.baseMipLevel = firstMip;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = firstArray;
		barrier.subresourceRange.layerCount = arrayCount;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		StageMask stageMask = decideTransPipelineBarrierStageMask(oldLayout, newLayout);

		barrier.srcAccessMask = stageMask.srcMask;
		barrier.dstAccessMask = stageMask.desMask;

		vkCmdPipelineBarrier(
			m_commandBuffer,
			stageMask.srcStage, stageMask.desStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		for (int i = 0; i < mipLevels; ++i)
			image.SetImageMipLayout(newLayout, firstMip + i);
	}
}