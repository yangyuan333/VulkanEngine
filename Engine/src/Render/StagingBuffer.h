#pragma once
#include "../RHI/Buffer.h"

namespace VulkanEngine
{
	class StagingBuffer
	{
	public:
		struct Allocation {
			uint32_t Size;
			uint32_t Offset;
		};
	public:
		StagingBuffer(size_t byteSize);
		Allocation Submit(const uint8_t* data, uint32_t byteSize);
		void Flush();
		void Reset();
	public:
		Buffer& GetBuffer() { return this->buffer; }
		const Buffer& GetBuffer() const { return this->buffer; }
		uint32_t GetCurrentOffset() const { return this->currentOffset; }
	private:
		Buffer buffer;
		uint32_t currentOffset;
	};
}