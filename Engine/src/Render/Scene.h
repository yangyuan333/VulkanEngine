#pragma once
#include "Camera.h"
#include "../Resource/ModelLoader.h"
#include "../Utility/SingleTon.h"
#include "GameObject.h"
#include "Light.h"
#include <memory>

namespace VulkanEngine
{
	// 收集所有的渲染对象
	class Scene : public Singleton<Scene>
	{
	friend class Singleton<Scene>;
	public:
		void AddModel(ModelData const& modelData);
		void AddGameObject(std::shared_ptr<GameObject> gameObject);
		void SetupCamera(std::shared_ptr<Camera> camera);
		void AddPointLight(PointLightComponent pointLight);
		void AddDirectionalLight(DirectionalLightComponent directionalLight);
		void UpdateLightBuffer(int frameIdx = -1);
	protected:
		Scene();
		~Scene();
		Scene(Scene const& other) = delete;
		Scene& operator=(Scene const& other) = delete;
	private:
		std::shared_ptr<Camera> m_camera;

		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
		
		std::vector<PointLightComponent> m_pointLights;
		DirectionalLightComponent m_directionalLight; // 强制为1吧，只能有一个 sun

		std::vector<std::shared_ptr<Buffer>> m_LightsBuffers; // directionalLight + pointLight + pointLight...
		std::vector<VkDescriptorSet> m_lightDescriptorSet;
		
	};
}