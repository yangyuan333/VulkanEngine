#include "Engine.h"
#include "../Utility/Config.h"
#include "../Render/RenderBackend.h"

namespace VulkanEngine
{
	Engine::Engine()
		: window(Config::WIDTH, Config::HEIGHT)
	{
		// ��Ҫ����������ʼ������ʱ Engine ��û�й�����ɣ�������ָ�뻹�ǿյģ�
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
		// ����˶������������ʼ�����ã�
		// ������Ⱦ��
		return;
    }

}