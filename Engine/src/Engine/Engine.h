#pragma once

#include "../Utility/SingleTon.h"
#include "../Render/Window.h"
#include "../Render/Renderer.h"
#include <iostream>
#include <memory>

namespace VulkanEngine
{
	/// <summary>
	/// init Vulkan; init Config;
	/// </summary>
	class Engine : public Singleton<Engine>
	{
		friend class Singleton<Engine>;
	public:
		inline GLFWwindow* GetWindowHandle() { return window.GetWindowHandle(); }
	public:
		~Engine();

	private:
		Engine();

	private:
		Window window;
		std::unique_ptr<Renderer> m_Renderer; // now there is only one renderer---defered renderer
	};
}