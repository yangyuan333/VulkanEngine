#include "Engine.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	Engine::Engine()
		: window(Config::WIDTH, Config::HEIGHT)
	{
		RenderBackend::Init();
		// ����˶������������ʼ�����ã�

	}
	Engine::~Engine()
	{
	}

}