#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <map>
#include "../Resource/ModelLoader.h"
// #include "../RHI/Buffer.h" // ���Ҳ���Ըɵ�
// #include "Material.h"
// #include "Light.h"

namespace VulkanEngine
{
	class Material;
	class Buffer;
	class LightCompoent;

	enum class GameObjectKind
	{
		Opaque,
		Transparent,
		// PointLight,
		// DirectionLight
	};

	struct ModelComponent
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(16) glm::mat3 modelMatrix_it;
	};

	struct TransformComponent {
		alignas(16) glm::vec3 location;
		alignas(16) glm::vec3 rotation;
		alignas(16) glm::vec3 scale;
	};

	class MeshComponent
	{
	public:
		MeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, ModelData::Material& material);
		MeshComponent(MeshComponent const& other) = delete;
		MeshComponent& operator=(MeshComponent const& other) = delete;
		~MeshComponent() = default;
	public:
		std::shared_ptr<Image> GetTetxure(std::string textureName) { return m_MaterialResource[textureName]; }
		std::shared_ptr<Sampler> GetSampler() { return m_sampler; }
		void BindVertex();
		void BindIndex();
		uint32_t GetIndexCnt() { return m_indexCnt; }
	private:
		void CreateMaterialTexture(ModelData::Material& material);
	private:
		std::shared_ptr<Buffer> m_vertexBuffer{nullptr};
		std::shared_ptr<Buffer> m_indexBuffer{nullptr};
		uint32_t m_vertexCnt;
		uint32_t m_indexCnt;
		std::map<std::string, std::shared_ptr<Image>> m_MaterialResource;
		std::shared_ptr<Sampler> m_sampler;
	};

	// Ӧ��Ҳ�ð���Ⱦ��Դ��װ���������---DescriptorSet---ÿ��Material�ֱ��Ӧ
	class GameObject
	{
	public:
		GameObject(GameObjectKind objectKind = GameObjectKind::Opaque);
		~GameObject();
		GameObject(GameObject const& other) = delete;
		GameObject& operator=(GameObject const& other) = delete;
	public:
		void SetupMeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, ModelData::Material& material);
		void SetupMeshComponent(std::shared_ptr<MeshComponent> meshComponent);
		void SetupTransform(TransformComponent const& transform); // ��������������������漰����̬������˵��
		// void SetupPointLight(PointLightComponent const& pointLight);
		// void SetupDirectionalLight(DirectionalLightComponent const& directioanlLight);
		void SetMaterial(std::shared_ptr<Material> material);
		void UpdateTransform(TransformComponent const& transform); // ��������������������漰����̬������˵��
		// void UpdatePointLight(PointLightComponent const& pointLight);
		// void UpdateDirectionalLight(DirectionalLightComponent const& directioanlLight);
		auto& GetDescriptorSets() { return m_descriptorSets; }
		auto& GetModelBuffers() { return m_buffers; }
		auto GetMeshTexture(std::string textureName) { return m_mesh->GetTetxure(textureName); }
		std::shared_ptr<Sampler> GetSampler() { return m_mesh->GetSampler(); }
	public:
		GameObjectKind GetGameObjectKind() const { return m_objectKind; }
		glm::mat4 GetMofelMatrix() const { return m_modelMatrix; }
		glm::mat4 GetMofelMatrix() { return m_modelMatrix; }
	public:
		void BindVertex();
		void BindIndex();
		void BindDescriptorSet(MaterialType type, int subpass);
		void Draw();
	private:
		glm::mat4 ComputeModelMatrix();
	private:
		GameObjectKind m_objectKind;
		TransformComponent m_transform;
		std::shared_ptr<MeshComponent> m_mesh;
		std::map<MaterialType, std::shared_ptr<Material>> m_materials; // ������ʣ�ÿ�����ʶ�Ӧһ��RenderPass---ShadowMap��PBR��TAA��ÿ��renderpass�������ԴҲ������

		glm::mat4 m_modelMatrix;

		// PointLightComponent m_pointLight;
		// DirectionalLightComponent m_directionalLight;
		// �����и��Ե� descriptor set��renderpass pipeline ��Ӧ��
	private:
		std::map<MaterialType, std::vector<std::map<int, VkDescriptorSet>>> m_descriptorSets;
		std::vector<std::shared_ptr<Buffer>> m_buffers; // ��������������ĳ� push_constant
	};
}