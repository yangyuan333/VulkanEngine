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
		// 不要在这里做初始化，此时 Engine 还没有构造完成，单例的指针还是空的；
	}
	void Engine::RenderTick(float ts)
	{
		RenderBackend::GetInstance().StartFrame();
		m_Renderer->Render(Scene::GetInstance());
		RenderBackend::GetInstance().EndFrame();
	}
	void Engine::LogicTick(float fs)
	{
		// 目前只更新 Camera；
		// 后续可以对 Object、Light 进行更新；
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
		// 这里后续需要将 DescriptorAllocator.Init 放到这里来做；根据 Renderer 和 Scene 来决定申请多大的 Pool；
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
		// 创建渲染器
		m_Renderer = std::make_shared<DeferredRenderer>(); // framebuffer 也一起创建了
    }

}