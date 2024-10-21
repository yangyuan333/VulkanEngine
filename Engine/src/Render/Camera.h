#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include <memory>

namespace VulkanEngine
{
	class Buffer;
	struct CameraComponent
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
		alignas(16) glm::vec3 viewPos;
	};

	class Camera
	{
	public:
		Camera();
	public:
		const glm::mat4& GetProjection() { return projection; }
		const glm::mat4& GetView() { return view; }
		const glm::mat4& GetInverseProjection() const { return inverseProjection; }
		const glm::mat4& GetInverseView() const { return inverseView; }
		const glm::vec3& GetPosition() const { return position; }

		virtual bool OnUpdate(float ts) = 0;
		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		void BindDescriptorSet(VkPipelineLayout pipelineLayout, int setIndex);

	protected:
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseProjection{ 1.0f };
		glm::mat4 inverseView{ 1.0f };
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };

		glm::vec2 lastMousePosition{ 0.0f, 0.0f };

		float verticalFOV;
		float nearClip;
		float farClip;

		uint32_t viewportWidth = 0, viewportHeight = 0;

		std::vector<std::shared_ptr<Buffer>> m_cameraBuffers;
		std::vector<VkDescriptorSet> m_cameraDescriptorSet;
	};

	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float verticalFOV, float nearClip, float farClip);

		bool OnUpdate(float ts) override;
		void OnResize(uint32_t width, uint32_t height) override;

		const float GetRotationSpeed() const { return m_rotationSpeed; }

		const glm::vec3& GetDirection() const { return m_forwardDirection; }

	private:
		void RecalculateProjection();
		void RecalculateView();

	private:
		glm::vec3 m_forwardDirection{ 0.0f, 0.0f, 0.0f };
		const float m_translationSpeed = 1000.0f;
		const float m_rotationSpeed = 0.3f;
	};
}