#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <map>
#include "../Utility/Config.h"
#include "Image.h"
#include "Sampler.h"
#include "Buffer.h"

namespace VulkanEngine
{
	struct ShaderImageDesc {
		std::shared_ptr<Image> image;
		VkImageUsageFlags usage;
		std::shared_ptr<Sampler> sampler;
	};

	struct ShaderBufferDesc {
		std::shared_ptr<Buffer> buffer;
		size_t offset;
		size_t range;
	};

	struct ShadowConfig
	{
		VkBool32 depthClampEnable;
		VkBool32 depthBiasEnable;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
	};

	struct CullConfig
	{
		VkCullModeFlags cullMode;
		VkFrontFace frontFace;
	};

	struct DepthStencilConfig
	{
		VkBool32 depthTestEnable;
		VkBool32 depthWriteEnable;
		VkCompareOp depthCompareOp;
		VkBool32 stencilTestEnable;
		VkStencilOpState stencilFront;
		VkStencilOpState stencilBack;
	};

	struct ColorAttachmentBlendConfig
	{
		VkBool32 blendEnable;
		VkBlendOp colorBlendOp;
		VkBlendFactor srcColorBlendFactor;
		VkBlendFactor dstColorBlendFactor;
		VkBlendOp alphaBlendOp;
		VkBlendFactor srcAlphaBlendFactor;
		VkBlendFactor dstAlphaBlendFactor;
	};

	struct ColorBlendConfig
	{
		std::vector<ColorAttachmentBlendConfig> colorAttachmentConfig;
	};

	struct PipelineConfig
	{
		std::string vert_spir_path;
		std::string frag_spir_path;
		std::vector<VkDynamicState> dynamicStates;
		// 这个后续得注意一下---全屏后处理这种怎么做
		VkVertexInputBindingDescription vertexBindingDescription; // 这里先处理成只能有一个 VAO 的情况，大多数情况都能用
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		VkPrimitiveTopology primitiveTopology;
		CullConfig cullConfig;
		ShadowConfig shadowConfig;
		VkSampleCountFlagBits msaaSampleCout;
		DepthStencilConfig depthStencilConifg;
		ColorBlendConfig colorBlendConfig;
	};

	/// <summary>
	/// 目前只涉及到图形管线，计算管线再看
	/// </summary>
	class Pipeline
	{
	public:
		struct BindMetaData {
			uint32_t set;
			uint32_t binding;
			uint32_t count;
			VkDescriptorType descriptorType;
			VkShaderStageFlags shaderStageFlag;
		};

	public:
		Pipeline(PipelineConfig const& pipelineConfig, VkRenderPass renderPass, uint32_t subpssIndex);
		~Pipeline();
		
		// 非及时更新，收集好了后，调用一次 update，节省cpu gpu
		// 后续如果对每个对象拥有一个set的话，这里可以优化
		void ClearBinds();
		void Bind(const std::string& resourceName, const ShaderBufferDesc& bufferDesc);
		void Bind(const std::string& resourceName, const ShaderImageDesc& imageDesc);
		void Bind(const std::string& resourceName, const std::vector<Image>& textureArray);
		void Bind(const std::string& resourceName, std::shared_ptr<Sampler> sampler);
		void UpdateBinds();
	private:
		void CreateShader(std::string const& vert_spir_path, std::string const& frag_spir_path);
		void GenerateVulkanDescriptorSetLayout();
		void GeneratePushConstantData();
		void CollectSpirvMetaData(std::vector<char> spivrBinary, VkShaderStageFlags shaderFlags);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

	private:
		VkPipeline m_pipeline;
		VkShaderModule m_vertexShader; // 好像没必要，创建后就销毁了
		VkShaderModule m_fragShader; // 同上
		std::array<std::map<uint32_t, VkDescriptorSet>, Config::MAX_FRAMES_IN_FLIGHT> m_descriptorSets; // 目前先采用单一的set，每个物体对象共用一个set，渲染时实时对其进行update
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		VkPipelineLayout m_pipelineLayout;

		std::unordered_map<std::string, BindMetaData> m_bindDatas; // 后续可以借助 spir-v 的反射自动从shader中获取资源
		std::vector<VkWriteDescriptorSet> m_currentWriteSets;
	};
}