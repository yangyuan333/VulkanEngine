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
		PointLight,
		DirectionLight
	};

	struct TransformComponent {
		alignas(16) glm::vec3 location;
		alignas(16) glm::vec3 rotation;
		alignas(16) glm::vec3 scale;
	};

	class MeshComponent
	{
	public:
		MeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::shared_ptr<ModelData::Material> material);
	private:
		void CreateMaterialTexture(std::shared_ptr<ModelData::Material> material);
	private:
		std::shared_ptr<Buffer> m_vertexBuffer{nullptr};
		std::shared_ptr<Buffer> m_indexBuffer{nullptr};
		uint32_t m_vertexCnt;
		uint32_t m_indexCnt;
		std::map<std::string, std::shared_ptr<Image>> m_MaterialResource;
	};

	class GameObject
	{
	public:
		GameObject(GameObjectKind objectKind = GameObjectKind::Opaque);
		~GameObject();
		GameObject(GameObject const& other) = delete;
		GameObject& operator=(GameObject const& other) = delete;
	private:
		TransformComponent m_transform;
		std::shared_ptr<MeshComponent> m_mesh;
		std::vector<std::shared_ptr<Material>> m_materials; // ������ʣ�ÿ�����ʶ�Ӧһ��RenderPass---ShadowMap��PBR��TAA��ÿ��renderpass�������ԴҲ������
		std::shared_ptr<LightCompoent> m_light;
		GameObjectKind m_objectKind;
		// �����и��Ե� descriptor set��renderpass pipeline ��Ӧ��
	};
}