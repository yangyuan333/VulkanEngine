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
	public:
		void Run();
		void Init();
	private:
		Engine();
		void RenderTick();
		void LogicTick();
		void CreateRenderer(); // 这里初始化延迟渲染管线---添加各种 RenderPass，和对应的pipeline(包括透明物体、opaque物体两种)，RenderPass里面暂存一下Pipeline，作为一个合集，供场景导入后，不同物体进行选择；
		void LoadScene(); // 涉及 灯光配置、场景导入
		void ConfigureLights();
		void ConfigureGameObject(); // 封装一层 GameObject、Component；


	private:
		Window window;
		std::unique_ptr<Renderer> m_Renderer; // now there is only one renderer---defered renderer
	};
}