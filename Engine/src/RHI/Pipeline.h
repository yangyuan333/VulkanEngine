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
		Pipeline(std::string const& vert_spir_path, std::string const& frag_spir_path, VkSampleCountFlagBits msaaSampleCout, PipelineConfig const& pipelineConfig, VkRenderPass renderPass, uint32_t subpssIndex);
		~Pipeline();
		
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
		VkShaderModule m_vertexShader;
		VkShaderModule m_fragShader;
		std::vector<std::map<uint32_t, VkDescriptorSet>> m_descriptorSets;
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		VkPipelineLayout m_pipelineLayout;

		std::unordered_map<std::string, BindMetaData> m_bindDatas;
		std::vector<VkWriteDescriptorSet> m_currentWriteSets;
	};
}