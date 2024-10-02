#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

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
	struct Vertex 
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Tangent{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Bitangent{ 0.0f, 0.0f, 0.0f };
		glm::vec2 TexCoord{ 0.0f, 0.0f };

		static VkVertexInputBindingDescription GetInputBindingDescription() 
		{
			return VkVertexInputBindingDescription{};
		};

		static std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions() 
		{
			return {};
		}
	};
	class MeshComponent
	{
	public:
		MeshComponent(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
	private:
		std::shared_ptr<Buffer> m_vertexBuffer{nullptr};
		std::shared_ptr<Buffer> m_indexBuffer{nullptr};
		uint32_t m_vertexCnt;
		uint32_t m_indexCnt;
	};

	class GameObject
	{
	private:
		TransformComponent m_transform;
		MeshComponent m_mesh;
		std::vector<std::shared_ptr<Material>> m_materials; // ������ʣ�ÿ�����ʶ�Ӧһ��RenderPass---ShadowMap��PBR��TAA��ÿ��renderpass�������ԴҲ������
		std::shared_ptr<LightCompoent> m_light;
		GameObjectKind objectKind;
		// �����и��Ե� descriptor set��renderpass pipeline ��Ӧ��
	};
}