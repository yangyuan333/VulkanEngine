#include "Buffer.h"
#include "../Render/RenderBackend.h"
#include "../Utility/UtilityFunc.h"

namespace VulkanEngine
{

	Buffer::Buffer(Buffer&& other) {
		m_buffer = other.m_buffer;
		m_byteSize = other.m_byteSize;
		m_bufferMemory = other.m_bufferMemory;
		m_mappedMemory = other.m_mappedMemory;

		other.m_buffer = VK_NULL_HANDLE;
		other.m_byteSize = 0;
		other.m_bufferMemory = VK_NULL_HANDLE;
		other.m_mappedMemory = nullptr;
	}

	Buffer& Buffer::operator=(Buffer&& other) {
		Destroy();

		m_buffer = other.m_buffer;
		m_byteSize = other.m_byteSize;
		m_bufferMemory = other.m_bufferMemory;
		m_mappedMemory = other.m_mappedMemory;

		other.m_buffer = VK_NULL_HANDLE;
		other.m_byteSize = 0;
		other.m_bufferMemory = VK_NULL_HANDLE;
		other.m_mappedMemory = nullptr;

		return *this;
	}

	Buffer::~Buffer()
	{
		Destroy();
	}

	Buffer::Buffer(size_t byteSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryUsage)
		: m_byteSize(byteSize), m_usage(usage), m_memoryUsage(memoryUsage)
	{
		Init(byteSize, usage, memoryUsage);
	}

	void Buffer::Init(size_t byteSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryUsage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = byteSize;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.flags = 0;

		if (vkCreateBuffer(RenderBackend::GetInstance().GetDevice(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(RenderBackend::GetInstance().GetDevice(), m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
			memRequirements.memoryTypeBits,
			memoryUsage);

		if (vkAllocateMemory(RenderBackend::GetInstance().GetDevice(), &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}
		// 注意这里的 offset 参数设置
		vkBindBufferMemory(RenderBackend::GetInstance().GetDevice(), m_buffer, m_bufferMemory, 0);
	}

	bool Buffer::IsMemoryMapped() const
	{
		return m_mappedMemory != nullptr;
	}

	void* Buffer::MapMemory()
	{
		if (m_memoryUsage & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			if (IsMemoryMapped()) UnmapMemory();
			vkMapMemory(RenderBackend::GetInstance().GetDevice(), m_bufferMemory, 0, m_byteSize, 0, &m_mappedMemory);
			return m_mappedMemory;
		}
		throw std::runtime_error("Buffer Memory Current Must be VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT to Map!");
	}

	void Buffer::UnmapMemory()
	{
		if (m_memoryUsage & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && IsMemoryMapped())
		{
			vkUnmapMemory(RenderBackend::GetInstance().GetDevice(), m_bufferMemory);
			m_mappedMemory = nullptr;
		}
	}

	void Buffer::FlushMemory()
	{
		if (m_memoryUsage & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) return;
		throw std::runtime_error("Buffer Memory Current Must be VK_MEMORY_PROPERTY_HOST_COHERENT_BIT to Flush!");
	}

	void Buffer::FlushMemory(size_t byteSize, size_t offset)
	{
		if (m_memoryUsage & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) return;
		throw std::runtime_error("Buffer Memory Current Must be VK_MEMORY_PROPERTY_HOST_COHERENT_BIT to Flush!");
	}
	
	void Buffer::WriteData(const uint8_t* data, size_t byteSize, size_t offset)
	{
		if (!IsMemoryMapped())
		{
			MapMemory();
			std::memcpy((void*)((uint8_t*)m_mappedMemory + offset), (const void*)data, byteSize);
			FlushMemory();
			UnmapMemory();
			return;
		}
		std::memcpy((void*)((uint8_t*)m_mappedMemory + offset), (const void*)data, byteSize);
	}
	void Buffer::WriteDataWithFlush(const uint8_t* data, size_t byteSize, size_t offset)
	{
		WriteData(data, byteSize, offset);
		if (IsMemoryMapped()) FlushMemory(byteSize, offset);
	}


	void Buffer::Destroy()
	{
		if (m_buffer != VK_NULL_HANDLE) {
			if (IsMemoryMapped()) UnmapMemory();

			vkDestroyBuffer(RenderBackend::GetInstance().GetDevice(), m_buffer, nullptr);
			vkFreeMemory(RenderBackend::GetInstance().GetDevice(), m_bufferMemory, nullptr);
			m_buffer = VK_NULL_HANDLE;
			m_bufferMemory = VK_NULL_HANDLE;
		}
	}
}