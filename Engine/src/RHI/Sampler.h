#pragma once

#include <vulkan/vulkan.h>

namespace VulkanEngine
{
	class Sampler
	{
	public:
		Sampler() = default;
		~Sampler();
		Sampler(Sampler&& other);
		Sampler& operator=(Sampler&& other);
		Sampler(VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uvwAddress, VkSamplerMipmapMode mipFilter);
		void Init(VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uvwAddress, VkSamplerMipmapMode mipFilter);

		const VkSampler& GetSamplerHandle() const;
	private:
		void Destroy();
	private:
		VkSampler m_sampler;
	};
}