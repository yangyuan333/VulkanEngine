#include "Scene.h"

namespace VulkanEngine
{
	void Scene::AddModel(ModelData const& modelData)
	{
		// Ҫ����ת�ɴ���Ⱦ��GameObject��
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
	{
		// Ĭ�϶�����Ⱦ����
		m_gameObjects.push_back(gameObject);
	}

	void Scene::SetupCamera(std::shared_ptr<Camera> camera)
	{
	
	}

	void Scene::AddPointLight(std::shared_ptr<GameObject> pointLight)
	{
	
	}

	void Scene::AddDirectionalLight(std::shared_ptr<GameObject> directionalLight)
	{
	
	}
}