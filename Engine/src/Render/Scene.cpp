#include "Scene.h"
#include "../Utility/Config.h"
#include "RenderBackend.h"

namespace VulkanEngine
{
	Scene::Scene()
	{
		// Light
		m_LightsBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);
		size_t bufferSize = sizeof(DirectionalLightComponent) + Config::MAX_PointLight_Num * sizeof(PointLightComponent);
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
			pointbufferInfo.range = sizeof(PointLightComponent) * Config::MAX_PointLight_Num;

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
		for (int pointIdx = 0; pointIdx < m_pointLights.size(); ++pointIdx)
		{
			m_LightsBuffers[frameIdx]->WriteData((uint8_t*)(&m_pointLights[pointIdx]), sizeof(PointLightComponent), offset);
			offset += sizeof(PointLightComponent);
		}
		m_LightsBuffers[frameIdx]->FlushMemory();

	}
}