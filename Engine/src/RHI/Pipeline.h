#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <map>
#include "../Render/RenderResource.h"

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
		VkBlendOp colorBlendOp = VK_BLEND_OP_ADD;
		VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD;
		VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	};

	struct ColorBlendConfig
	{
		std::vector<ColorAttachmentBlendConfig> colorAttachmentConfig;
	};

	struct PipelineConfig
	{
		std::vector<VkDynamicState> dynamicStates;
		VkVertexInputBindingDescription vertexBindingDescription;
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		VkPrimitiveTopology primitiveTopology;
		CullConfig cullConfig;
		ShadowConfig shadowConfig;
		DepthStencilConfig depthStencilConifg;
		ColorBlendConfig colorBlendConfig;
	};

	class Pipeline
	{
		friend class RenderPass;
	public:
		struct BindMetaData {
			uint32_t set;
			uint32_t binding;
			uint32_t count;
			VkDescriptorType descriptorType;
			VkShaderStageFlags shaderStageFlag;
		};

	public:
		Pipeline(std::string const& vert_spir_path, std::string const& frag_spir_path, VkSampleCountFlagBits msaaSampleCout, PipelineConfig const& pipelineConfig, VkRenderPass renderPass, uint32_t subpssIndex);
		~Pipeline();
		Pipeline(Pipeline const& other) = delete;
		Pipeline& operator=(Pipeline const& other) = delete;
		Pipeline(Pipeline&& other);
		Pipeline& operator=(Pipeline&& other);

		// void ClearBinds();
		// void Bind(const std::string& resourceName, const ShaderBufferDesc& bufferDesc);
		// void Bind(const std::string& resourceName, const ShaderImageDesc& imageDesc);
		// void Bind(const std::string& resourceName, const std::vector<Image>& textureArray);
		// void Bind(const std::string& resourceName, std::shared_ptr<Sampler> sampler);
		// void UpdateBinds();

	public:
		VkPipeline GetPipelineHandle() const { return m_pipeline; }
	private:
		void CreateShader(std::string const& vert_spir_path, std::string const& frag_spir_path);
		void GenerateVulkanDescriptorSetLayout();
		void GeneratePushConstantData();
		void CollectSpirvMetaData(std::vector<char> spivrBinary, VkShaderStageFlags shaderFlags);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		void Destroy();
	private:
		VkPipeline m_pipeline;
		VkShaderModule m_vertexShader;
		VkShaderModule m_fragShader;
		// std::vector<std::map<uint32_t, VkDescriptorSet>> m_descriptorSets;
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		VkPipelineLayout m_pipelineLayout;

		std::unordered_map<std::string, BindMetaData> m_bindDatas;
		std::vector<VkWriteDescriptorSet> m_currentWriteSets;
	};
}