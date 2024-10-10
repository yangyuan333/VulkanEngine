#include "GameObject.h"
#include "../RHI/CommandBuffer.h"
#include "RenderBackend.h"
#include "../Utility/UtilityFunc.h"
#include "Material.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	GameObject::GameObject(GameObjectKind objectKind)
		: m_objectKind(objectKind)
	{
		size_t bufferSize = sizeof(ModelComponent);
		m_buffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_buffers.push_back(
				std::make_shared<Buffer>(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			);
			m_buffers[frameIdx]->MapMemory();
		}

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

    void GameObject::SetupTransform(TransformComponent const& transform)
    {
		m_transform = transform;
		ComputeModelMatrix();
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_buffers[frameIdx]->WriteDataWithFlush((uint8_t*)&m_modelMatrix, sizeof(m_modelMatrix));
		}
    }
	/*
	void GameObject::SetupPointLight(PointLightComponent const& pointLight)
	{
		if (m_objectKind == GameObjectKind::PointLight)
			m_pointLight = pointLight;
		else
			throw std::runtime_error("Only PointLight Object could Set PointLightComponent!");

		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_buffers[frameIdx]->WriteDataWithFlush((uint8_t*)&m_pointLight, sizeof(m_pointLight));
		}
	}
	*/
	/*
	void GameObject::SetupDirectionalLight(DirectionalLightComponent const& directioanlLight)
	{
		if (m_objectKind == GameObjectKind::DirectionLight)
			m_directionalLight = directioanlLight;
		else
			throw std::runtime_error("Only DirectionLight Object could Set DirectionalLightComponent!");
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_buffers[frameIdx]->WriteDataWithFlush((uint8_t*)&m_directionalLight, sizeof(m_directionalLight));
		}
	}
	*/
	void GameObject::SetMaterial(std::shared_ptr<Material> material)
	{
		// 这里要根据material的类型---获取descritorsetlayout，并生成descriptorset，并绑定数据
		m_materials[material->GetMaterialType()] = material;
		// 生成 descriptor set，并且 bind 渲染资源；关键是放到哪里做呢？ 放到 Pipeline 中做，加个 BindObject 方法；
		// 这里只生成，不绑定，绑定还是放到 RenderPass 里面做；
		auto& pipelines = material->GetPipelines();
		std::vector<std::vector<VkDescriptorSet>> pipelinesDescriptorSets; pipelinesDescriptorSets.reserve(pipelines.size());
		for (auto& pipeline : pipelines)
		{
			std::vector<VkDescriptorSet> pipelineDescriptorSets;
			
		}
	}

	void GameObject::UpdateTransform(TransformComponent const& transform)
	{
		m_transform = transform;
		ComputeModelMatrix();
		ModelComponent modelComponent;
		modelComponent.modelMatrix = m_modelMatrix;
		modelComponent.modelMatrix_it = glm::inverse(m_modelMatrix);
		m_buffers[RenderBackend::GetInstance().GetCurrentFrameIndex()]->WriteDataWithFlush((uint8_t*)&modelComponent, sizeof(modelComponent));
	}
	/*
	void GameObject::UpdatePointLight(PointLightComponent const& pointLight)
	{
		m_pointLight = pointLight;
		m_buffers[RenderBackend::GetInstance().GetCurrentFrameIndex()]->WriteDataWithFlush((uint8_t*)&m_pointLight, sizeof(m_pointLight));
	}
	*/
	/*
	void GameObject::UpdateDirectionalLight(DirectionalLightComponent const& directioanlLight)
	{
		m_directionalLight = directioanlLight;
		m_buffers[RenderBackend::GetInstance().GetCurrentFrameIndex()]->WriteDataWithFlush((uint8_t*)&m_directionalLight, sizeof(m_directionalLight));
	}
	*/
	void GameObject::BindPipeline()
	{
		
	}

	glm::mat4 GameObject::ComputeModelMatrix()
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_transform.location);
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_transform.rotation.x), glm::vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_transform.rotation.y), glm::vec3(0, 1, 0));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_transform.rotation.z), glm::vec3(0, 0, 1));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_transform.scale);
		glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		return modelMatrix;
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
		m_sampler = std::make_shared<Sampler>(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR);
	}
}