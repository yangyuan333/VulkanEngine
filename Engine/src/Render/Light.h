#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace VulkanEngine
{
	// �ȷ���ʵ�֣�Light��Ϣ����¼�� Scene �У�ͳһ����һ�� DescritorSet����������� ShadowMap ��ʱ����ô���أ�
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
}