#pragma once

#include <vulkan/vulkan.h>
#include <utility>
#include <span>
#include "../Render/RenderResource.h"
#include "RenderPass.h"
#include "FrameBuffer.h"

namespace VulkanEngine
{
	class CommandBuffer
	{
	public:
		CommandBuffer(VkCommandBuffer commandBuffer) : m_handle(std::move(commandBuffer)) {}

		const auto& GetNativeHandle() const { return m_handle; }
		void Begin();
		void End();

		void Draw(uint32_t vertexCount, uint32_t instanceCount);

		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
			uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
			uint32_t vertexOffset, uint32_t firstInstance);

		void SetViewport(const Viewport& viewport);
		void SetScissor(const Rect2D& scissor);
		void SetRenderArea(const Image& image);

		void Dispatch(uint32_t x, uint32_t y, uint32_t z);

		void BeginRenderPass(RenderPass renderPass, FrameBuffer frameBuffer);
		void EndRenderPass();

		void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout layout,
			std::vector<VkDescriptorSet>& descriptorSets); // 这个后续用到 dynamic uniform buffer 的时候得再架构;

		void CopyImage(
			const Image& source, VkImageUsageFlagBits src_usage, uint32_t src_mipLevel, uint32_t src_layer,
			const Image& destination, VkImageUsageFlagBits des_usage, uint32_t des_mipLevel, uint32_t des_layer);
		void CopyBufferToImage(
			const Buffer& source, uint32_t offset,
			const Image& destination, VkImageUsageFlagBits des_usage, uint32_t des_mipLevel = 0, uint32_t des_layer = 0);
		void CopyImageToBuffer(
			const Image& source, VkImageUsageFlagBits src_usage, uint32_t src_mipLevel, uint32_t src_layer,
			const Buffer& destination, uint32_t offset);
		void CopyBuffer(
			const Buffer& source, uint32_t src_offset,
			const Buffer& destination, uint32_t des_offset,
			size_t byteSize);

		void BlitImage(
			const Image& source, VkImageUsageFlagBits sourceUsage, 
			const Image& destination, VkImageUsageFlagBits destinationUsage,
			VkFilter filter);
		void GenerateImageMipmap(const Image& image, VkImageUsageFlagBits initialUsage, VkFilter filter);
		
		void TransferLayout(const Image& image, VkImageUsageFlagBits oldLayout, VkImageUsageFlagBits newLayout);
		
		void BindPipeline(const Pipeline& pipeline);

		void BindVertexBuffers(const Buffer& buffer, uint32_t binding, uint32_t offset = 0);
		void BindIndexBufferUInt32(const Buffer& indexBuffer);
		void BindIndexBufferUInt16(const Buffer& indexBuffer);

	private:
		VkCommandBuffer m_handle;
	};
}