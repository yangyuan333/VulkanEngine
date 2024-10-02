#include "Camera.h"
#include "InputManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VulkanEngine
{
	EditorCamera::EditorCamera(float verticalFOV, float nearClip, float farClip)
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