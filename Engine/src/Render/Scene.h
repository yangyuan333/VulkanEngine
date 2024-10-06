#pragma once
#include "Camera.h"
#include "GameObject.h"
#include <memory>

namespace VulkanEngine
{
	// �ռ����е���Ⱦ����
	class Scene
	{
	private:
		std::shared_ptr<Camera> m_camera;
		std::vector<GameObject> m_gameObjects;
		std::vector<GameObject> m_pointLights;
		GameObject m_directionalLight; // ǿ��Ϊ1�ɣ�ֻ����һ�� sun

	};
}