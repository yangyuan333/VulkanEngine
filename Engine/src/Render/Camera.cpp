#include "Camera.h"
#include "InputManager.h"
#include "../Utility/Config.h"
#include "RenderBackend.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VulkanEngine
{
	Camera::Camera()
	{
		m_cameraBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);
		size_t bufferSize = sizeof(CameraComponent);
		for (int frameIdx = 0; frameIdx < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdx)
		{
			m_cameraBuffers.push_back(std::make_shared<Buffer>(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
			m_cameraBuffers[frameIdx]->MapMemory();
		}
		VkDescriptorSetLayout cameraLayout;
		VkDescriptorSetLayoutBinding uboCameraLayoutBinding{};
		uboCameraLayoutBinding.binding = 0;
		uboCameraLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboCameraLayoutBinding.descriptorCount = 1; // UBO Array
		uboCameraLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
		uboCameraLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo cameralayoutInfo{};
		cameralayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		cameralayoutInfo.bindingCount = 1;
		cameralayoutInfo.pBindings = &uboCameraLayoutBinding;
		if (vkCreateDescriptorSetLayout(RenderBackend::GetInstance().GetDevice(), &cameralayoutInfo, nullptr, &cameraLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		m_cameraDescriptorSet.resize(Config::MAX_FRAMES_IN_FLIGHT);
		for (auto frameIdex = 0; frameIdex < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdex)
		{
			m_cameraDescriptorSet[frameIdex] = (RenderBackend::GetInstance().GetDescriptorAllocator()->Allocate(cameraLayout));

			VkDescriptorBufferInfo camerabufferInfo{};
			camerabufferInfo.buffer = m_cameraBuffers[frameIdex]->GetBufferHandle();
			camerabufferInfo.offset = 0;
			camerabufferInfo.range = sizeof(CameraComponent);

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_cameraDescriptorSet[frameIdex];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &camerabufferInfo;
			descriptorWrites[0].pImageInfo = nullptr; // Optional
			descriptorWrites[0].pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(RenderBackend::GetInstance().GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	EditorCamera::EditorCamera(float verticalFOV, float nearClip, float farClip)
		:Camera()
	{
		nearClip = nearClip;
		farClip = farClip;
		verticalFOV = verticalFOV;

		m_forwardDirection = glm::vec3(0, 0, -1);
		position = glm::vec3(0, 0, 6);
	}

	bool EditorCamera::OnUpdate(float ts)
	{
		// 这里跑通后重写
		static auto inputManger = InputManger::GetInstance();
		glm::vec2 mousePos = inputManger->GetMousePosition();
		glm::vec2 delta = (mousePos - lastMousePosition) * 0.002f;
		lastMousePosition = mousePos;

		if (!inputManger->IsMouseButtonDown(MouseButton::Right)) {
			inputManger->SetCursorMode(CursorMode::Normal);
			return false;
		}

		inputManger->SetCursorMode(CursorMode::Locked);

		bool moved = false;

		constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDirection = glm::cross(m_forwardDirection, upDirection);

		// Movement
		if (inputManger->IsKeyDown(KeyCode::W)) {
			position += m_forwardDirection * m_translationSpeed * ts;
			moved = true;
		}
		else if (inputManger->IsKeyDown(KeyCode::S)) {
			position -= m_forwardDirection * m_translationSpeed * ts;
			moved = true;
		}
		if (inputManger->IsKeyDown(KeyCode::A)) {
			position -= rightDirection * m_translationSpeed * ts;
			moved = true;
		}
		else if (inputManger->IsKeyDown(KeyCode::D)) {
			position += rightDirection * m_translationSpeed * ts;
			moved = true;
		}
		if (inputManger->IsKeyDown(KeyCode::Q)) {
			position -= upDirection * m_translationSpeed * ts;
			moved = true;
		}
		else if (inputManger->IsKeyDown(KeyCode::E)) {
			position += upDirection * m_translationSpeed * ts;
			moved = true;
		}

		// Rotation
		if (delta.x != 0.0f || delta.y != 0.0f) {
			float pitchDelta = delta.y * GetRotationSpeed();
			float yawDelta = delta.x * GetRotationSpeed();

			glm::quat q =
				glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
					glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));

			m_forwardDirection = glm::rotate(q, m_forwardDirection);

			moved = true;
		}

		if (moved) RecalculateView();

		CameraComponent cameraData;
		cameraData.view = GetView();
		cameraData.proj = GetProjection();
		cameraData.viewproj = cameraData.proj * cameraData.view;
		cameraData.viewPos = GetPosition();

		m_cameraBuffers[RenderBackend::GetInstance().GetCurrentFrameIndex()]->WriteDataWithFlush((uint8_t*)&cameraData, sizeof(cameraData), 0);

		return moved;
	}

	void EditorCamera::OnResize(uint32_t width, uint32_t height)
	{
		if (width == viewportWidth && height == viewportHeight) return;

		viewportWidth = width;
		viewportHeight = height;

		RecalculateProjection();
	}
	
	void EditorCamera::RecalculateProjection()
	{
		projection = glm::perspective(glm::radians(verticalFOV), (float)viewportWidth / (float)viewportHeight, nearClip, farClip);
		projection[1][1] *= -1.0f;
		inverseProjection = glm::inverse(projection);
	}

	void EditorCamera::RecalculateView()
	{
		view = glm::lookAt(position, position + m_forwardDirection, glm::vec3(0, 1, 0));
		inverseView = glm::inverse(view);
	}
}