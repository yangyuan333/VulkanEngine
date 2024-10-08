#pragma once
#include "Camera.h"
#include "../Resource/ModelLoader.h"
#include "../Utility/SingleTon.h"
#include "GameObject.h"
#include <memory>

namespace VulkanEngine
{
	// �ռ����е���Ⱦ����
	class Scene : public Singleton<Scene>
	{
	friend class Singleton<Scene>;
	public:
		void AddModel(ModelData const& modelData);
		void AddGameObject(std::shared_ptr<GameObject> gameObject);
		void SetupCamera(std::shared_ptr<Camera> camera);
		void AddPointLight(std::shared_ptr<GameObject> pointLight);
		void AddDirectionalLight(std::shared_ptr<GameObject> directionalLight);
	private:
		std::shared_ptr<Camera> m_camera;
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		std::vector<std::shared_ptr<GameObject>> m_pointLights;
		std::shared_ptr<GameObject> m_directionalLight; // ǿ��Ϊ1�ɣ�ֻ����һ�� sun

	};
}