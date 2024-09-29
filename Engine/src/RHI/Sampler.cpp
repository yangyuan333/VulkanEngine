#include "Sampler.h"
#include "../Render/RenderBackend.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	Sampler::~Sampler()
	{
		Destroy();
	}

	Sampler::Sampler(Sampler&& other)
	{
		m_sampler = other.m_sampler;
		other.m_sampler = VK_NULL_HANDLE;
	}

	Sampler& Sampler::operator=(Sampler&& other)
	{
		Destroy();
		m_sampler = other.m_sampler;

		other.m_sampler = VK_NULL_HANDLE;
		
		return *this;
	}

	Sampler::Sampler(VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uvwAddress, VkSamplerMipmapMode mipFilter)
	{
		Init(minFilter, magFilter, uvwAddress, mipFilter);
	}

	void Sampler::Init(VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uvwAddress, VkSamplerMipmapMode mipFilter)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = minFilter;
		samplerInfo.magFilter = magFilter;
		samplerInfo.addressModeU = uvwAddress;
		samplerInfo.addressModeV = uvwAddress;
		samplerInfo.addressModeW = uvwAddress;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.anisotropyEnable = Config::Enable_Anisotropy;
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(RenderBackend::GetInstance().GetPhyDevice(), &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.unnormalizedCoordinates = VK_FALSE; // 这个得后续用到再优化
		samplerInfo.compareEnable = VK_FALSE; // 这个等后续写到 ShadowMap 的时候再优化
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = mipFilter;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1000; // 给个足够大的数值
		if (vkCreateSampler(RenderBackend::GetInstance().GetDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	const VkSampler& Sampler::GetSamplerHandle() const
	{
		return m_sampler;
	}

	void Sampler::Destroy()
	{
		if (m_sampler != VK_NULL_HANDLE) vkDestroySampler(RenderBackend::GetInstance().GetDevice(), m_sampler, nullptr);
		m_sampler = VK_NULL_HANDLE;
	}
}