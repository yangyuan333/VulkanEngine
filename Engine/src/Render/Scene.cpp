#include "Scene.h"
#include "../Utility/Config.h"
#include "RenderBackend.h"
#include "Material.h"
#include "PbrDeferredPass.h"
#include "../Engine/Engine.h"

namespace VulkanEngine
{
	Scene::Scene()
	{
		// Light
		m_LightsBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);
		size_t bufferSize = sizeof(DirectionalLightComponent) + sizeof(uint32_t)*4 + Config::MAX_PointLight_Num * sizeof(PointLightComponent);
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_LightsBuffers.push_back(std::make_shared<Buffer>(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
			m_LightsBuffers[frameIdx]->MapMemory();
		}
		VkDescriptorSetLayout lightLayout;
		VkDescriptorSetLayoutBinding uboSunLightLayoutBinding{};
		uboSunLightLayoutBinding.binding = 0;
		uboSunLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboSunLightLayoutBinding.descriptorCount = 1; // UBO Array
		uboSunLightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uboSunLightLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutBinding uboPointLightLayoutBinding{};
		uboPointLightLayoutBinding.binding = 1;
		uboPointLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboPointLightLayoutBinding.descriptorCount = 1; // UBO Array
		uboPointLightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uboPointLightLayoutBinding.pImmutableSamplers = nullptr; // Optional

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboSunLightLayoutBinding, uboPointLightLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();
		if (vkCreateDescriptorSetLayout(RenderBackend::GetInstance().GetDevice(), &layoutInfo, nullptr, &lightLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		m_lightDescriptorSet.resize(Config::MAX_FRAMES_IN_FLIGHT);
		for (auto frameIdex = 0; frameIdex < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdex)
		{
			m_lightDescriptorSet[frameIdex] = (RenderBackend::GetInstance().GetDescriptorAllocator()->Allocate(lightLayout));

			VkDescriptorBufferInfo sunbufferInfo{};
			sunbufferInfo.buffer = m_LightsBuffers[frameIdex]->GetBufferHandle();
			sunbufferInfo.offset = 0;
			sunbufferInfo.range = sizeof(DirectionalLightComponent);

			VkDescriptorBufferInfo pointbufferInfo{};
			pointbufferInfo.buffer = m_LightsBuffers[frameIdex]->GetBufferHandle();
			pointbufferInfo.offset = sizeof(DirectionalLightComponent);
			pointbufferInfo.range = sizeof(uint32_t) * 4 + sizeof(PointLightComponent) * Config::MAX_PointLight_Num;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_lightDescriptorSet[frameIdex];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &sunbufferInfo;
			descriptorWrites[0].pImageInfo = nullptr; // Optional
			descriptorWrites[0].pTexelBufferView = nullptr; // Optional

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_lightDescriptorSet[frameIdex];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &pointbufferInfo;
			descriptorWrites[1].pImageInfo = nullptr; // Optional
			descriptorWrites[1].pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(RenderBackend::GetInstance().GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	Scene::~Scene()
	{

	}

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
		m_camera = camera;

		CameraComponent cameraData;
		cameraData.view = m_camera->GetView();
		cameraData.proj = m_camera->GetProjection();
		cameraData.viewproj = cameraData.proj * cameraData.view;
		cameraData.viewPos = m_camera->GetPosition();
		for (int index = 0; index < Config::MAX_FRAMES_IN_FLIGHT; ++index)
		{
			m_camera->UpdateUniformData(cameraData, index);
		}
	}

	void Scene::AddPointLight(PointLightComponent pointLight)
	{
		m_pointLights.push_back(pointLight);
	}

	void Scene::AddDirectionalLight(DirectionalLightComponent directionalLight)
	{
		m_directionalLight = directionalLight;
	}

	void Scene::UpdateLightBuffer(int frameIdx)
	{
		if (frameIdx == -1)
		{
			for (frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
			{
				size_t offset = 0;
				m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&m_directionalLight), sizeof(m_directionalLight), 0); offset+= sizeof(m_directionalLight);
				uint32_t pointLightNum = m_pointLights.size();
				m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&pointLightNum), sizeof(uint32_t)*4, offset); offset += (sizeof(uint32_t)*4);
				for (int pointIdx = 0; pointIdx < m_pointLights.size(); ++pointIdx)
				{
					m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&m_pointLights[pointIdx]), sizeof(PointLightComponent), offset);
					offset += sizeof(PointLightComponent);
				}
				m_LightsBuffers[frameIdx]->FlushMemory();
			}
			return;
		}

		size_t offset = 0;
		m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&m_directionalLight), sizeof(m_directionalLight), 0); offset += sizeof(m_directionalLight);
		uint32_t pointLightNum = m_pointLights.size();
		m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&pointLightNum), sizeof(uint32_t)*4, offset); offset += (sizeof(uint32_t)*4);
		for (int pointIdx = 0; pointIdx < m_pointLights.size(); ++pointIdx)
		{
			m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&m_pointLights[pointIdx]), sizeof(PointLightComponent), offset);
			offset += sizeof(PointLightComponent);
		}
		m_LightsBuffers[frameIdx]->FlushMemory();
	}
	void Scene::LoadGLTFScene(std::string const& filePath)
	{
		// 从 filePath 导入场景
		ModelData SceneData = ModelLoader::LoadFromGltf(filePath);

		for (auto& object : SceneData.Shapes)
		{
			std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObjectKind::Opaque);
			gameObject->SetupMeshComponent(object.Vertices, object.Indices, SceneData.Materials[object.MaterialIndex]);
			gameObject->SetupTransform(TransformComponent{ {0,0,0},{0,0,0},{1,1,1} });
			// 构建 Material，后续多个 renderpass，需要把对应的material都给加上
			std::shared_ptr<Material> pbr_material = std::make_shared<Material>(MaterialType::DeferredPassMaterial, Engine::GetInstance().GetRenderer()->GetRenderPass(RenderPassEnum::PbrDeferredPass));
			gameObject->SetMaterial(pbr_material);
			m_gameObjects.push_back(gameObject);
			// TODO:这里其实还要统计descriptorset的数量，用来构建descriptorpool；
		}
	}
	void Scene::InitScene()
	{
		// 灯光设置
		DirectionalLightComponent sun;
		sun.lightPos = 20.0f * glm::vec3(87.26932, 455.97552, -49.665558);
		sun.direction = glm::normalize(-1.0f * sun.lightPos); // 这里要注意一下，和shader对应，别反了
		sun.radiance = glm::vec3{ 1.0f, 1.0f, 1.0f };
		sun.ligthColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
		AddDirectionalLight(sun);

		glm::vec3 startPos = glm::vec3(200, 200, 200);
		float offset = 200;
		int iter = 2;
		for (int i = -iter; i < iter; ++i) {
			for (int j = -iter; j < iter; ++j) {
				for (int k = -iter; k < iter; ++k) {
					glm::vec3 lightPos = glm::vec3(startPos.x + i * offset, startPos.y + j * offset, startPos.z + k * offset);
					glm::vec3 intensity{ 10.0f };
					glm::vec3 lightColor = glm::vec3(1, 1, 1) * glm::vec3(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f));
					PointLightComponent light{ lightPos, intensity, lightColor };
					AddPointLight(light);
				}
			}
		}
		UpdateLightBuffer(); // 这个可以放到后面的 Update 中去做；目前先放这里，当成静态光照环境；
		// Camera设置
		auto camera = std::make_shared<EditorCamera>(Config::VerticalFOV, Config::NearClip, Config::FarClip);
		SetupCamera(camera);
	}

	void Scene::BindLightsDescriptorSet(VkPipelineLayout pipelineLayout, int setIndex)
	{
		vkCmdBindDescriptorSets(
			RenderBackend::GetInstance().GetCurrentFrame().Commands.GetCommandBufferHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout, setIndex, 1, &m_lightDescriptorSet[RenderBackend::GetInstance().GetCurrentFrameIndex()], 0, nullptr);
	}
}