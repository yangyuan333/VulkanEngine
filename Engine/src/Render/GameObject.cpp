#include "GameObject.h"
#include "../RHI/CommandBuffer.h"
#include "RenderBackend.h"
#include "../Utility/UtilityFunc.h"

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

	void GameObject::SetupMeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::shared_ptr<ModelData::Material> material)
	{
		if (m_objectKind == GameObjectKind::Opaque || m_objectKind == GameObjectKind::Transparent)
			m_mesh = std::make_shared<MeshComponent>(vertices, indices, material);
		else
			throw std::runtime_error("Only Opaque or Transparent Object could Set Mesh!");
	}

	void GameObject::SetupMeshComponent(std::shared_ptr<MeshComponent> meshComponent)
	{
		if (m_objectKind == GameObjectKind::Opaque || m_objectKind == GameObjectKind::Transparent)
			m_mesh = meshComponent;
		else
			throw std::runtime_error("Only Opaque or Transparent Object could Set MeshComponent!");
	}

    void GameObject::UpdateTransform(TransformComponent const& transform)
    {
		m_transform = transform;
    }

	void GameObject::SetupPointLight(PointLightComponent const& pointLight)
	{
		if (m_objectKind == GameObjectKind::PointLight)
			m_pointLight = pointLight;
		else
			throw std::runtime_error("Only PointLight Object could Set PointLightComponent!");
	}

	void GameObject::SetupDirectionalLight(DirectionalLightComponent const& directioanlLight)
	{
		if (m_objectKind == GameObjectKind::DirectionLight)
			m_directionalLight = directioanlLight;
		else
			throw std::runtime_error("Only DirectionLight Object could Set DirectionalLightComponent!");
	}

	void GameObject::SetMaterial(std::shared_ptr<Material> material)
	{
		m_materials.push_back(material);
	}

	void MeshComponent::CreateMaterialTexture(std::shared_ptr<ModelData::Material> material)
	{
		// Albedo
		std::shared_ptr<Image> albedoTexture = std::make_shared<Image>();
		FillImage(
			*albedoTexture, material->AlbedoTexture, 
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ImageOptions::MIPMAPS);
		m_MaterialResource["albedo"] = albedoTexture;
		// Normal
		std::shared_ptr<Image> normalTexture = std::make_shared<Image>();
		FillImage(
			*normalTexture, material->NormalTexture,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ImageOptions::MIPMAPS);
		m_MaterialResource["normal"] = normalTexture;
		// MetallicRoughness
		std::shared_ptr<Image> metallicRoughnessTexture = std::make_shared<Image>();
		FillImage(
			*metallicRoughnessTexture, material->MetallicRoughness,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ImageOptions::MIPMAPS);
		m_MaterialResource["metallicRoughness"] = metallicRoughnessTexture;
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

		// ImageTexture
		CreateMaterialTexture(material);
	}
}