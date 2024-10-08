#include "GameObject.h"
#include "../RHI/CommandBuffer.h"
#include "RenderBackend.h"

namespace VulkanEngine
{
	GameObject::GameObject(GameObjectKind objectKind)
		: m_objectKind(objectKind)
	{
		
	}

	GameObject::~GameObject()
	{
		// TODO
	}

	MeshComponent::MeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::shared_ptr<ModelData::Material> material)
		: m_vertexCnt(vertices.size()), m_indexCnt(indices.size()) 
	{
		VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * m_vertexCnt;
		VkDeviceSize indexBufferSize = sizeof(indices[0]) * m_indexCnt;

		m_vertexBuffer = std::make_shared<Buffer>(
			vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_indexBuffer = std::make_shared<Buffer>(
			indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CommandBuffer cmdBuffer = RenderBackend::GetInstance().BeginSingleTimeCommand();
		// 这里后续得优化
		auto& stagingBuffer = RenderBackend::GetInstance().GetStagingBuffer();
		StagingBuffer::Allocation allocationVertex = stagingBuffer.Submit((uint8_t*)vertices.data(), vertexBufferSize);
		StagingBuffer::Allocation allocationIndex = stagingBuffer.Submit((uint8_t*)indices.data(), indexBufferSize);
		stagingBuffer.Flush();

		cmdBuffer.CopyBuffer(stagingBuffer.GetBuffer(), allocationVertex.Offset, *m_vertexBuffer, 0, allocationVertex.Size);
		cmdBuffer.CopyBuffer(stagingBuffer.GetBuffer(), allocationIndex.Offset, *m_vertexBuffer, 0, allocationIndex.Size);

		RenderBackend::GetInstance().SubmitSingleTimeCommand(cmdBuffer.GetCommandBufferHandle());
		stagingBuffer.Reset();

	}
}