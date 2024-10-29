#pragma once

#include "../Utility/SingleTon.h"
#include "../Render/Window.h"
#include "../Render/Renderer.h"
#include <iostream>
#include <memory>

#include "renderdoc_app.h"

namespace VulkanEngine
{
	class Scene;
	/// <summary>
	/// init Vulkan; init Config;
	/// </summary>
	class Engine : public Singleton<Engine>
	{
		friend class Singleton<Engine>;
	public:
		inline GLFWwindow* GetWindowHandle() { return m_window.GetWindowHandle(); }
		inline auto& GetRenderer() { return m_Renderer; }
		void UpdateWindowSize(int width, int height);
	public:
		~Engine();
	public:
		void Run();
		void Init();
	private:
		Engine();
		void RenderTick(float ts);
		void LogicTick(float fs);
		float CalculateDeltaTime();
		// void CreateRenderer(); // �����ʼ���ӳ���Ⱦ����---��Ӹ��� RenderPass���Ͷ�Ӧ��pipeline(����͸�����塢opaque��������)��RenderPass�����ݴ�һ��Pipeline����Ϊһ���ϼ�������������󣬲�ͬ�������ѡ��
		// void LoadScene(); // �漰 �ƹ����á���������
		// void ConfigureLights();
		// void ConfigureGameObject(); // ��װһ�� GameObject��Component��


	private:
		Window m_window;
		std::shared_ptr<Renderer> m_Renderer; // now there is only one renderer---defered renderer
		// std::shared_ptr<Scene> m_Scene;
		std::chrono::steady_clock::time_point m_lastTickTimePoint{ std::chrono::steady_clock::now() };

	public:
		RENDERDOC_API_1_1_2* rdoc_api = nullptr;
	};
}