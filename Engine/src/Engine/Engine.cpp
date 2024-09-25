#include "Engine.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	Engine::Engine()
		: window(Config::WIDTH, Config::HEIGHT)
	{
		
	}
	Engine::~Engine()
	{
	}

}