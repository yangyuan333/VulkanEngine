#pragma once

#include <memory>
#include <mutex>
#include <utility>

namespace VulkanEngine
{ 
	template <typename T>
	class Singleton {
	public:
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		template <typename... Args>
		static T& GetInstance(Args&&... args) {
			std::call_once(initFlag, [&]() {
				instance.reset(new T(std::forward<Args>(args)...));
				});
			return *instance;
		}

	protected:
		Singleton() = default;
		~Singleton() = default;

	private:
		static std::unique_ptr<T> instance;
		static std::once_flag initFlag;
	};

	template <typename T>
	std::unique_ptr<T> Singleton<T>::instance = nullptr;

	template <typename T>
	std::once_flag Singleton<T>::initFlag;
}