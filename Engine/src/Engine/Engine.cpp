#include "Engine.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"
#include "../Render/DeferRenderer.h"
#include "../Render/InputManager.h"
#include <chrono>

namespace VulkanEngine
{
	Engine::Engine()
		: m_window(Config::WIDTH, Config::HEIGHT)
	{
		// ��Ҫ����������ʼ������ʱ Engine ��û�й�����ɣ�������ָ�뻹�ǿյģ�
	}
	void Engine::RenderTick(float ts)
	{
		RenderBackend::GetInstance().StartFrame();
		m_Renderer->Render(Scene::GetInstance());
		RenderBackend::GetInstance().EndFrame();
	}
	void Engine::LogicTick(float fs)
	{
		// Ŀǰֻ���� Camera��
		// �������Զ� Object��Light ���и��£�
		auto& camera = Scene::GetInstance().GetCamera();
		camera->OnResize(m_window.GetWidth(), m_window.GetHeight());
		camera->OnUpdate(fs);
	}
	float Engine::CalculateDeltaTime()
	{
		float dalta;
		{
			using namespace std::chrono;
			steady_clock::time_point tickTimePoint = steady_clock::now();
			auto timeSpan = std::chrono::duration_cast<duration<float>>(tickTimePoint - m_lastTickTimePoint);
			dalta = timeSpan.count();

			m_lastTickTimePoint = tickTimePoint;
		}
		return dalta;
	}

	void Engine::UpdateWindowSize(int width, int height)
	{
		m_window.UpdateSize(width, height);
	}

	Engine::~Engine()
	{
		
	}

	void Engine::Run()
	{
		Scene::GetInstance().InitScene();
		Scene::GetInstance().LoadGLTFScene(Config::GetInstance().SceneFile);
		// ���������Ҫ�� DescriptorAllocator.Init �ŵ��������������� Renderer �� Scene ������������� Pool��
		while (!glfwWindowShouldClose(m_window.GetWindowHandle()))
		{
			glfwPollEvents();
			float fs = CalculateDeltaTime();
			LogicTick(fs);
			RenderTick(fs);
		}
		vkDeviceWaitIdle(RenderBackend::GetInstance().GetDevice());
	}

    void Engine::Init()
    {
		RenderBackend::Init();
		InputManger::Init(m_window.GetWindowHandle());
		// ������Ⱦ��
		m_Renderer = std::make_shared<DeferredRenderer>(); // framebuffer Ҳһ�𴴽���
    }

}