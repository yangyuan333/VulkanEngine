#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <map>
#include "../Resource/ModelLoader.h"
// #include "../RHI/Buffer.h" // 这个也可以干掉
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
		PointLight,
		DirectionLight
	};

	struct TransformComponent {
		alignas(16) glm::vec3 location;
		alignas(16) glm::vec3 rotation;
		alignas(16) glm::vec3 scale;
	};

	// 这两不应该有 transform 信息，后面再修改，方位信息应该用 transformcomponent
	struct PointLightComponent {
		alignas(16)  glm::vec3 position;
		alignas(16)  glm::vec3 intensity;
		alignas(16)  glm::vec3 lightColor;
	};

	struct DirectionalLightComponent {
		alignas(16) glm::vec3 lightPos{};
		alignas(16) glm::vec3 direction{};
		alignas(16) glm::vec3 radiance{ 1.0f };
		alignas(16) glm::vec3 ligthColor{ 1.0f };
	};

	class MeshComponent
	{
	public:
		MeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::shared_ptr<ModelData::Material> material);
		MeshComponent(MeshComponent const& other) = delete;
		MeshComponent& operator=(MeshComponent const& other) = delete;
		~MeshComponent() = default;
	private:
		void CreateMaterialTexture(std::shared_ptr<ModelData::Material> material);
	private:
		std::shared_ptr<Buffer> m_vertexBuffer{nullptr};
		std::shared_ptr<Buffer> m_indexBuffer{nullptr};
		uint32_t m_vertexCnt;
		uint32_t m_indexCnt;
		std::map<std::string, std::shared_ptr<Image>> m_MaterialResource;
	};

	// 应该也得把渲染资源封装在这个里面---DescriptorSet---每个Material分别对应
	class GameObject
	{
	public:
		GameObject(GameObjectKind objectKind = GameObjectKind::Opaque);
		~GameObject();
		GameObject(GameObject const& other) = delete;
		GameObject& operator=(GameObject const& other) = delete;
	public:
		void SetupMeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::shared_ptr<ModelData::Material> material);
		void SetupMeshComponent(std::shared_ptr<MeshComponent> meshComponent);
		void SetupTransform(TransformComponent const& transform); // 这里先这样，后续如果涉及到动态物体再说；
		void SetupPointLight(PointLightComponent const& pointLight);
		void SetupDirectionalLight(DirectionalLightComponent const& directioanlLight);
		void SetMaterial(std::shared_ptr<Material> material);
		void UpdateTransform(TransformComponent const& transform); // 这里先这样，后续如果涉及到动态物体再说；
		void UpdatePointLight(PointLightComponent const& pointLight);
		void UpdateDirectionalLight(DirectionalLightComponent const& directioanlLight);
		auto& GetDescriptorSets() { return m_descriptorSets; }
		void BindPipeline();
	public:
		GameObjectKind GetGameObjectKind() const { return m_objectKind; }
		glm::mat4 GetMofelMatrix() const { return m_modelMatrix; }
		glm::mat4 GetMofelMatrix() { return m_modelMatrix; }
	private:
		glm::mat4 ComputeModelMatrix();
	private:
		TransformComponent m_transform;
		glm::mat4 m_modelMatrix;
		std::shared_ptr<MeshComponent> m_mesh;
		std::map<MaterialType, std::shared_ptr<Material>> m_materials; // 多个材质，每个材质对应一个RenderPass---ShadowMap、PBR、TAA，每个renderpass所需的资源也在里面
		PointLightComponent m_pointLight;
		DirectionalLightComponent m_directionalLight;
		GameObjectKind m_objectKind;
		// 这里有各自的 descriptor set，renderpass pipeline 对应；
	private:
		std::map<MaterialType, std::vector<std::map<int, VkDescriptorSet>>> m_descriptorSets;
		std::vector<std::shared_ptr<Buffer>> m_buffers; // 先这样做，后面改成 push_constant
	};
}