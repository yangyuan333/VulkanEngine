#pragma once

#include "../RHI/RenderPass.h"
#include "../Utility/Config.h"
#include <string>
#include <vector>

namespace VulkanEngine
{
	class PbrDeferredPass : public RenderPass
	{
	public:
		PbrDeferredPass();
		virtual ~PbrDeferredPass();
		PbrDeferredPass(PbrDeferredPass const& other) = delete;
		PbrDeferredPass& operator=(PbrDeferredPass const& other) = delete;
	public:
		virtual const std::unordered_map<std::string, TextureDesc>& GetPassTextureDescs();
		virtual void UpdatePassTextureDescsWidthHeight(uint32_t width, uint32_t height) const;
		virtual void Build() override;
		virtual std::vector<std::map<int, VkDescriptorSet>>& BindGameObject(std::shared_ptr<GameObject> object) override;
	public:
		void BeginRenderPass(FrameBuffer& frameBuffer) override;
		void EndRenderPass() override;
		void BindInputDescriptorSet(VkPipelineLayout pipelineLayout, int setIndex) override;
		virtual std::vector<VkDescriptorSet>& GetInputAttachmentDescriptorSet() { return m_inputAttachmentSets; }
	protected:
		std::vector<VkDescriptorSet> m_inputAttachmentSets;
	protected:
		mutable std::unordered_map<std::string, TextureDesc> m_pbrDeferredPassTextureDescs = {
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
				VK_FORMAT_R16G16_SFLOAT,
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

		const std::string geometry_vert_spir_path = "D:\\GraphicsLearning\\Code\\VulkanEngine\\Engine\\src\\Shader\\pbrBasePassVert.glsl.spv"; // 这里赋值
		const std::string geometry_frag_spir_path = "D:\\GraphicsLearning\\Code\\VulkanEngine\\Engine\\src\\Shader\\pbrBasePassFrag.glsl.spv"; // 这里赋值
		const std::string lighting_vert_spir_path = "D:\\GraphicsLearning\\Code\\VulkanEngine\\Engine\\src\\Shader\\fullScreenVert.glsl.spv"; // 这里赋值
		const std::string lighting_frag_spir_path = "D:\\GraphicsLearning\\Code\\VulkanEngine\\Engine\\src\\Shader\\pbrLightingFrag.glsl.spv"; // 这里赋值
	};
}