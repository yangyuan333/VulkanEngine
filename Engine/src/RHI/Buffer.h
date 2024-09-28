#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanEngine
{
	class Buffer
	{
	public:
		Buffer() = default;
		~Buffer();
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&& other);
		Buffer& operator=(Buffer&& other);
		
		Buffer(size_t byteSize, VkBufferUsageFlagBits usage, VkMemoryPropertyFlagBits memoryUsage);
		void Init(size_t byteSize, VkBufferUsageFlagBits usage, VkMemoryPropertyFlagBits memoryUsage);

		auto GetBufferHandle() const { return m_buffer; }
		auto GetByteSize() const { return m_byteSize; }
		bool IsMemoryMapped() const;

		void* MapMemory();
		void* MapMemory(size_t mapSize, size_t offset = 0); // 等后续用到多个 buffer 共用一个 memory 的时候再设计优化
		void UnmapMemory();
		void WriteData(const uint8_t* data, size_t byteSize, size_t offset = 0);
		void WriteDataWithFlush(const uint8_t* data, size_t byteSize, size_t offset = 0);

	private:
		void FlushMemory();
		void FlushMemory(size_t byteSize, size_t offset);
		void Destroy();
	private:

		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;

		VkBufferUsageFlagBits m_usage;
		VkMemoryPropertyFlagBits m_memoryUsage;

		void* m_mappedMemory{ nullptr };
		size_t m_byteSize{ 0 };
	};
}