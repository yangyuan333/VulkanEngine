#pragma once

#include "../RHI/RenderPass.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	class PbrDeferredPass : public RenderPass
	{
	public:
		virtual const std::unordered_map<std::string, TextureDesc>& GetPassTextureDescs();
		virtual const void UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height);
		virtual void Build() override;

	protected:
		std::unordered_map<std::string, TextureDesc> m_pbrDeferredPassTextureDescs = {
			{"SceneColor", TextureDesc{
				Config::SceneColorFormat,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}},
			{"SceneDepth", TextureDesc{
				VK_FORMAT_R8G8B8A8_UNORM ,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}},
			{"GBufferA", TextureDesc{
				VK_FORMAT_R16G16B16A16_SFLOAT ,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }},
			{"GBufferB", TextureDesc{
				VK_FORMAT_R8G8B8A8_UNORM ,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}},
			{"GBufferC", TextureDesc{
				VK_FORMAT_R8G8B8A8_UNORM ,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}},
			{"GBufferD", TextureDesc{
				VK_FORMAT_R8G8B8A8_UNORM ,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}} 
			};
	};
}