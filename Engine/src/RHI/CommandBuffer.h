#pragma once

#include <vulkan/vulkan.h>
#include <utility>
#include <span>
#include "../Render/RenderResource.h"
#include "RenderPass.h"
#include "FrameBuffer.h"

namespace VulkanEngine
{
	enum class CommandBeginFlag
	{
		Default,
		OneTime
	};

	enum class CommandFlag
	{
		Primary,
		Secondary
	};

	class CommandBuffer
	{
	public:
		CommandBuffer(CommandFlag commandFlag  = CommandFlag::Primary);
		CommandBuffer(VkCommandBuffer commandBuffer) : m_commandBuffer(std::move(commandBuffer)) {}
		~CommandBuffer();

		const auto& GetCommandBufferHandle() const { return m_commandBuffer; }
		void Begin(CommandBeginFlag commandFlag = CommandBeginFlag::Default);
		void End();
		void EndAndSubmitImmediate(VkQueue queue);
		void SubmitImmediate(VkQueue queue);
		void Submit(VkQueue queue); // 后续考虑同步量

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

		// 下面的函数后续都得优化
		// 改成两种模式：1. 立即模式；2. Record模式
		// 目前都先用立即模式处理，
		void CopyImage(
			VkQueue queue,
			const Image& source, uint32_t src_mipLevel, uint32_t src_layer,
			const Image& destination, uint32_t des_mipLevel, uint32_t des_layer);
		void CopyBufferToImage(
			VkQueue queue,
			const Buffer& source, uint32_t offset,
			const Image& destination, uint32_t des_mipLevel = 0, uint32_t des_layer = 0);
		void CopyImageToBuffer(
			VkQueue queue,
			const Image& source, uint32_t src_mipLevel, uint32_t src_layer,
			const Buffer& destination, uint32_t offset);
		void CopyBuffer(
			VkQueue queue,
			const Buffer& source, uint32_t src_offset,
			const Buffer& destination, uint32_t des_offset,
			size_t byteSize);

		void BlitImage(
			VkQueue queue,
			const Image& source,
			const Image& destination,
			VkFilter filter = VK_FILTER_LINEAR,
			uint32_t src_mipLevel = 0, uint32_t src_firstArray = 0, uint32_t src_arrayCount = 1,
			uint32_t des_mipLevel = 0, uint32_t des_firstArray = 0, uint32_t des_arrayCount = 1);
		void GenerateImageMipmap(VkQueue queue, const Image& image, VkImageUsageFlags initialUsage, VkFilter filter);
		
		void TransferLayout(VkQueue queue, const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t firstMip = 0, uint32_t mipLevels = 1, uint32_t firstArray = 0, uint32_t arrayCount = 1);
		
		void BindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

		void BindVertexBuffer(const Buffer& buffer, uint32_t binding, VkDeviceSize offset = 0);
		void BindVertexBuffers(std::vector<std::reference_wrapper<const Buffer>>const& buffers, uint32_t binding, std::vector<VkDeviceSize>const& offset = {});
		void BindIndexBufferUInt32(const Buffer& indexBuffer, VkDeviceSize offset = 0);
		void BindIndexBufferUInt16(const Buffer& indexBuffer, VkDeviceSize offset = 0);

		void Destroy();

	private:
		VkCommandBuffer m_commandBuffer;
	};
}