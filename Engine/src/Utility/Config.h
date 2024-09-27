#pragma once

#include "SingleTon.h"
#include "../RHI/Descriptor.h"
#include "../RHI/Pipeline.h"
#include <vector>

namespace VulkanEngine
{
	enum class RenderPassEnum
	{
		PbrDeferredPass, // v1.0
		ShadowMapPass, // v1.1
		TaaPass, // v2.0
		BloomPass // v2.1
	};

	class Config : public Singleton<Config>
	{
	public:
		// Windwos Config
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		static constexpr bool WINDOW_RESIZEABLE = false;
		// Render Config
		static constexpr bool ENABLE_MSAA = false;
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
		static constexpr VkFormat SceneColorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
#if _DEBUG
		static constexpr bool EnableValidationLayers = true;
#else
		static constexpr bool EnableValidationLayers = false;
#endif
	public:
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	public:
		const PoolSizes defaultPoolSize = PoolSizes{{
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER, 1},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 2},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1}
			}};
		const uint32_t defaultMaxSets = 2; // 占位，后续得更改

		// 只有 VK_FALSE 起作用了
		static constexpr ColorAttachmentBlendConfig opaqueColorAttachmentBlendConfig = ColorAttachmentBlendConfig{ VK_FALSE };

		const PipelineConfig opaqueScenePipelineConfig = {
			{ VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR },
			Vertex::GetInputBindingDescription(),
			Vertex::GetVertexInputAttributeDescriptions(),
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			{ VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE }, // 这个后面可以优化，封装成pipeline的函数
			{ VK_FALSE, VK_FALSE, 0.0f, 0.0f, 0.0f },
			{ VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, {}, {}  },
			{
				{opaqueColorAttachmentBlendConfig, opaqueColorAttachmentBlendConfig, opaqueColorAttachmentBlendConfig, opaqueColorAttachmentBlendConfig}
			}
		};

		const PipelineConfig fullScreenPipelineConfig = {
			{ VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR },
			{},
			{},
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			{ VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE }, // 这个后面可以优化，封装成pipeline的函数
			{ VK_FALSE, VK_FALSE, 0.0f, 0.0f, 0.0f },
			{ VK_FALSE, VK_FALSE, VK_COMPARE_OP_ALWAYS, VK_FALSE, {}, {}  },
			{
				{opaqueColorAttachmentBlendConfig}
			}
		};
	};
}