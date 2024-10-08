#include "Scene.h"

namespace VulkanEngine
{
	void Scene::AddModel(ModelData const& modelData)
	{
		// 要把它转成待渲染的GameObject；
	}

	void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
	{
		// 默认都是渲染物体
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