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
		// ���������ע��һ��---ȫ������������ô��
		VkVertexInputBindingDescription vertexBindingDescription; // �����ȴ����ֻ����һ�� VAO ���������������������
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		VkPrimitiveTopology primitiveTopology;
		CullConfig cullConfig;
		ShadowConfig shadowConfig;
		VkSampleCountFlagBits msaaSampleCout;
		DepthStencilConfig depthStencilConifg;
		ColorBlendConfig colorBlendConfig;
	};

	/// <summary>
	/// Ŀǰֻ�漰��ͼ�ι��ߣ���������ٿ�
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
		
		// �Ǽ�ʱ���£��ռ����˺󣬵���һ�� update����ʡcpu gpu
		// ���������ÿ������ӵ��һ��set�Ļ�����������Ż�
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
		VkShaderModule m_vertexShader; // ����û��Ҫ���������������
		VkShaderModule m_fragShader; // ͬ��
		std::array<std::map<uint32_t, VkDescriptorSet>, Config::MAX_FRAMES_IN_FLIGHT> m_descriptorSets; // Ŀǰ�Ȳ��õ�һ��set��ÿ�����������һ��set����Ⱦʱʵʱ�������update
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		VkPipelineLayout m_pipelineLayout;

		std::unordered_map<std::string, BindMetaData> m_bindDatas; // �������Խ��� spir-v �ķ����Զ���shader�л�ȡ��Դ
		std::vector<VkWriteDescriptorSet> m_currentWriteSets;
	};
}