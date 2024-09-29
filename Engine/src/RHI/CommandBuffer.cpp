#include "CommandBuffer.h"
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
	void CommandBuffer::Begin(CommandFlag commandFlag)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		switch (commandFlag)
		{
			case CommandFlag::Default:
			{
				beginInfo.flags = 0;
				break;
			}
			case CommandFlag::OneTime:
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
	void CommandBuffer::CopyImage(const Image& source, VkImageUsageFlagBits src_usage, uint32_t src_mipLevel, uint32_t src_layer, const Image& destination, VkImageUsageFlagBits des_usage, uint32_t des_mipLevel, uint32_t des_layer)
	{
		
	}
	void CommandBuffer::CopyBufferToImage(const Buffer& source, uint32_t offset, const Image& destination, VkImageUsageFlagBits des_usage, uint32_t des_mipLevel, uint32_t des_layer)
	{

	}
	void CommandBuffer::CopyImageToBuffer(const Image& source, VkImageUsageFlagBits src_usage, uint32_t src_mipLevel, uint32_t src_layer, const Buffer& destination, uint32_t offset)
	{

	}
	void CommandBuffer::CopyBuffer(const Buffer& source, uint32_t src_offset, const Buffer& destination, uint32_t des_offset, size_t byteSize)
	{

	}
	void CommandBuffer::BlitImage(const Image& source, VkImageUsageFlagBits sourceUsage, const Image& destination, VkImageUsageFlagBits destinationUsage, VkFilter filter)
	{

	}
	void CommandBuffer::GenerateImageMipmap(const Image& image, VkImageUsageFlagBits initialUsage, VkFilter filter)
	{

	}
	void CommandBuffer::TransferLayout(const Image& image, VkImageUsageFlagBits oldLayout, VkImageUsageFlagBits newLayout)
	{

	}
}