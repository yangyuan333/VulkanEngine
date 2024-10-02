#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "GLFW/glfw3.h"

#include <memory>

#include "KeyCodes.h"

namespace VulkanEngine {
	// 这个window先不和engine强绑定
	// 先这样写，后续想好了再和现有单例模式绑定
	class InputManger {
	public:
		InputManger(InputManger const& other) = delete;
		InputManger& operator=(InputManger const& other) = delete;
		InputManger(InputManger&& other) = delete;
		InputManger& operator=(InputManger&& other) = delete;
	public:
		InputManger(GLFWwindow* window) : m_windowHandle(window) {}
		
		bool IsKeyDown(KeyCode keycode);
		bool IsMouseButtonDown(MouseButton button);

		glm::vec2 GetMousePosition();

		void SetCursorMode(CursorMode mode);

		static void Init(GLFWwindow* window);
	public:
		static auto GetInstance() { return s_instance; }
	private:
		GLFWwindow* m_windowHandle;
		static std::shared_ptr<InputManger> s_instance;
	};
}