#include "InputManager.h"
#include "KeyCodes.h"

namespace VulkanEngine
{
	std::shared_ptr<InputManger> InputManger::s_instance{ nullptr };

	bool InputManger::IsKeyDown(KeyCode keycode)
	{
		int state = glfwGetKey(m_windowHandle, (int)keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool InputManger::IsMouseButtonDown(MouseButton button)
	{
		int state = glfwGetMouseButton(m_windowHandle, (int)button);
		return state == GLFW_PRESS;
	}

	glm::vec2 InputManger::GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(m_windowHandle, &x, &y);
		return { (float)x, (float)y };
	}

	void InputManger::SetCursorMode(CursorMode mode)
	{
		glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}

	void InputManger::Init(GLFWwindow* window)
	{
		s_instance = std::make_shared<InputManger>(window);
	}
}