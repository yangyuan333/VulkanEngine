#include "Engine.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	Engine::Engine()
		: window(Config::WIDTH, Config::HEIGHT)
	{
		// 不要在这里做初始化，此时 Engine 还没有构造完成，单例的指针还是空的；
	}
	Engine::~Engine()
	{
		
	}

	void Engine::Run()
	{
		
	}

    void Engine::Init()
    {
		RenderBackend::Init();
		// 相机运动控制在哪里初始化设置；
		// 创建渲染器
		return;
    }

}