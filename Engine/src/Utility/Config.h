#pragma once

#include "SingleTon.h"
#include "../RHI/Descriptor.h"
#include "../RHI/Pipeline.h"
#include "../Render/GameObject.h"
#include <vector>

namespace VulkanEngine
{
	class Config : public Singleton<Config>
	{
	friend class Singleton<Config>;
	public:
		// Windows Config
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		static constexpr bool WINDOW_RESIZEABLE = false;
		// Render Config
		static constexpr bool ENABLE_MSAA = false;
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
		static constexpr size_t MAX_STAGINGBUFFER_SIZE = 64 * 1024 * 1024;
		static constexpr size_t MAX_PointLight_Num = 64;
		VkFormat SceneColorFormat;//  = VK_FORMAT_R16G16B16A16_SFLOAT; // 这个得和swapchain格式保持一致
		static constexpr bool Enable_Anisotropy = VK_TRUE;
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
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 8},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1}
			}};
		
		const uint32_t defaultMaxSets = 2;
		
		static constexpr ColorAttachmentBlendConfig opaqueColorAttachmentBlendConfig = {
			VK_FALSE
		};

		const PipelineConfig opaqueScenePipelineConfig = {
			{ VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR },
			Vertex::GetInputBindingDescription(),
			Vertex::GetVertexInputAttributeDescriptions(),
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			{ VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE },
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
			{ VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE },
			{ VK_FALSE, VK_FALSE, 0.0f, 0.0f, 0.0f },
			{ VK_FALSE, VK_FALSE, VK_COMPARE_OP_ALWAYS, VK_FALSE, {}, {}  },
			{
				{opaqueColorAttachmentBlendConfig}
			}
		};

		static constexpr float VerticalFOV = 45.0f;
		static constexpr float NearClip = 0.1f;
		static constexpr float FarClip = 1000.0f;

		const std::string SceneFile = "../../Assets/Scene/Sponza/glTF/Sponza.gltf";
	};
}