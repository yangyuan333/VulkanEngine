#include "StagingBuffer.h"

namespace VulkanEngine
{
	StagingBuffer::StagingBuffer(size_t byteSize)
		: buffer(
			byteSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 
		currentOffset(0)
	{
		buffer.MapMemory();
	}

	StagingBuffer::Allocation StagingBuffer::Submit(const uint8_t* data, uint32_t byteSize)
	{
		buffer.WriteData(data, byteSize, currentOffset);
		currentOffset += byteSize;
		return StagingBuffer::Allocation{ byteSize, currentOffset - byteSize };
	}

	void StagingBuffer::Flush()
	{ 
		buffer.FlushMemory(currentOffset, 0);
	}

	void StagingBuffer::Reset()
	{
		currentOffset = 0;
	}
}