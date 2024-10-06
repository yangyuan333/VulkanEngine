#pragma once
#include "Camera.h"
#include "GameObject.h"
#include <memory>

namespace VulkanEngine
{
	// 收集所有的渲染对象
	class Scene
	{
	private:
		std::shared_ptr<Camera> m_camera;
		std::vector<GameObject> m_gameObjects;
		std::vector<GameObject> m_pointLights;
		GameObject m_directionalLight; // 强制为1吧，只能有一个 sun

	};
}