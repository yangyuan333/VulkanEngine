#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace VulkanEngine
{
	class Window
	{
	public:
		Window(uint32_t width, uint32_t height, std::string title = "Vulkan Engine");
		~Window();
		
		Window& operator=(Window const& other) = delete;
		Window(Window const& other) = delete;
	public:
		inline GLFWwindow* GetWindowHandle() { return m_window; }
		inline uint32_t GetWidth() { return m_width; }
		inline uint32_t GetHeight() { return m_height; }
	public:
		void OnUpdate(float fs);
	private:
		GLFWwindow* m_window;

		uint32_t m_width, m_height;
		std::string m_title;
	};
}