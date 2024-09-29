#pragma once

#include <vulkan/vulkan.h>
#include <utility>
#include <span>
#include "../Render/RenderResource.h"
#include "RenderPass.h"
#include "FrameBuffer.h"

namespace VulkanEngine
{
	enum class CommandFlag
	{
		Default,
		OneTime
	};

	class CommandBuffer
	{
	public:
		CommandBuffer(VkCommandBuffer commandBuffer) : m_commandBuffer(std::move(commandBuffer)) {}

		const auto& GetCommandBufferHandle() const { return m_commandBuffer; }
		void Begin(CommandFlag commandFlag = CommandFlag::Default);
		void End();

		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1);

		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
			uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
			uint32_t vertexOffset, uint32_t firstInstance);

		void SetViewport(const Viewport& viewport);
		void SetScissor(const Rect2D& scissor);

		void BeginRenderPass(RenderPass const& renderPass);
		void EndRenderPass();

		void BindDescriptorSet(
			VkPipelineLayout layout, std::vector<VkDescriptorSet>& descriptorSets, uint32_t firstIndex, 
			VkPipelineBindPoint bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS); // 这个后续用到 dynamic uniform buffer 的时候得再架构;

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
		
		void BindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

		void BindVertexBuffer(const Buffer& buffer, uint32_t binding, VkDeviceSize offset = 0);
		void BindVertexBuffers(std::vector<std::reference_wrapper<const Buffer>>const& buffers, uint32_t binding, std::vector<VkDeviceSize>const& offset = {});
		void BindIndexBufferUInt32(const Buffer& indexBuffer, VkDeviceSize offset = 0);
		void BindIndexBufferUInt16(const Buffer& indexBuffer, VkDeviceSize offset = 0);

	private:
		VkCommandBuffer m_commandBuffer;
	};
}