#pragma once

#include "../Utility/SingleTon.h"
#include <iostream>

namespace VulkanEngine
{
	class Engine : public Singleton<Engine>
	{
		friend class Singleton<Engine>;

	public:
		~Engine()
		{
			std::cout << "Engine Dout" << std::endl;
		}

	private:
		Engine()
		{
			std::cout << "Engine Cout" << std::endl;
		}
		Engine(int i)
		{
			std::cout << "Engine Cout 1" << std::endl;
		}

	};
}