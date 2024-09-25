#include "Window.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	Window::Window(uint32_t width, uint32_t height, std::string title)
		:m_width(width), m_height(height), m_title(title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		if (Config::WINDOW_RESIZEABLE) 
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
	}

	void Window::OnUpdate(float fs)
	{
		glfwPollEvents();
	}
}