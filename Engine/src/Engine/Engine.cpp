#include "Engine.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	Engine::Engine()
		: window(Config::WIDTH, Config::HEIGHT)
	{
		RenderBackend::Init();
		// 相机运动控制在哪里初始化设置；
		// 创建渲染器

	}
	Engine::~Engine()
	{
		
	}

	void Engine::Run()
	{
		
	}

}