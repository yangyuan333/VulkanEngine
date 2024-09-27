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
		void CreateRenderer(); // �����ʼ���ӳ���Ⱦ����---��Ӹ��� RenderPass���Ͷ�Ӧ��pipeline(����͸�����塢opaque��������)��RenderPass�����ݴ�һ��Pipeline����Ϊһ���ϼ�������������󣬲�ͬ�������ѡ��
		void LoadScene(); // �漰 �ƹ����á���������
		void ConfigureLights();
		void ConfigureGameObject(); // ��װһ�� GameObject��Component��


	private:
		Window window;
		std::unique_ptr<Renderer> m_Renderer; // now there is only one renderer---defered renderer
	};
}