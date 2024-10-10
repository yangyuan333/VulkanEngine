#include "Pipeline.h"
#include <iostream>
#include <fstream>
#include <spirv_cross/spirv_glsl.hpp>
#include <utility>
#include <map>
#include <GLFW/glfw3.h>
#include "../Render/RenderBackend.h"
#include "../Engine/Engine.h"
#include "../Utility/Config.h"

namespace VulkanEngine
{
	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(RenderBackend::GetInstance().GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}

	void Pipeline::Destroy()
	{
		if (m_pipeline != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(RenderBackend::GetInstance().GetDevice(), m_fragShader, nullptr);
			vkDestroyShaderModule(RenderBackend::GetInstance().GetDevice(), m_vertexShader, nullptr);
			vkDestroyPipeline(RenderBackend::GetInstance().GetDevice(), m_pipeline, nullptr);
			vkDestroyPipelineLayout(RenderBackend::GetInstance().GetDevice(), m_pipelineLayout, nullptr);
			for (auto& layout : m_descriptorSetLayouts)
			{
				vkDestroyDescriptorSetLayout(RenderBackend::GetInstance().GetDevice(), layout, nullptr);
				layout = VK_NULL_HANDLE;
			}
			m_pipeline = VK_NULL_HANDLE;
			m_pipelineLayout = VK_NULL_HANDLE;
			m_fragShader = VK_NULL_HANDLE;
			m_vertexShader = VK_NULL_HANDLE;
		}
	}

	std::array<int,2> GetCurrentWindowSize()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(Engine::GetInstance().GetWindowHandle(), &width, &height);
		return {width, height};
	}

	Pipeline::Pipeline(std::string const& vert_spir_path, std::string const& frag_spir_path, VkSampleCountFlagBits msaaSampleCout, PipelineConfig const& pipelineConfig, VkRenderPass renderPass, uint32_t subpssIndex)
	{
		// m_descriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);

		CreateShader(vert_spir_path, frag_spir_path);

		// Pipeline Config
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_vertexShader;
		vertShaderStageInfo.pName = "main"; // 后续可以通过反射获取
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_fragShader;
		fragShaderStageInfo.pName = "main"; // 后续可以通过反射获取
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(pipelineConfig.dynamicStates.size());
		dynamicState.pDynamicStates = pipelineConfig.dynamicStates.data();
		
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		if (pipelineConfig.vertexAttributeDescriptions.size() == 0)
		{
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
		}
		else
		{
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &(pipelineConfig.vertexBindingDescription); // Optional
			vertexInputInfo.vertexAttributeDescriptionCount = pipelineConfig.vertexAttributeDescriptions.size();
			vertexInputInfo.pVertexAttributeDescriptions = pipelineConfig.vertexAttributeDescriptions.data(); // Optional
		}
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = pipelineConfig.primitiveTopology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		auto windowSize = GetCurrentWindowSize();
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)windowSize[0];
		viewport.height = (float)windowSize[1];
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { (uint32_t)windowSize[0], (uint32_t)windowSize[1] };
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// cullMode, frontFace, depthClampEnable depthBiasEnable depthBiasConstantFactor depthBiasClamp depthBiasSlopeFactor
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = pipelineConfig.cullConfig.cullMode;
		rasterizer.frontFace = pipelineConfig.cullConfig.frontFace;
		rasterizer.depthClampEnable = pipelineConfig.shadowConfig.depthClampEnable;
		rasterizer.depthBiasEnable = pipelineConfig.shadowConfig.depthBiasEnable;
		rasterizer.depthBiasConstantFactor = pipelineConfig.shadowConfig.depthBiasConstantFactor; // Optional
		rasterizer.depthBiasClamp = pipelineConfig.shadowConfig.depthBiasClamp; // Optional
		rasterizer.depthBiasSlopeFactor = pipelineConfig.shadowConfig.depthBiasSlopeFactor; // Optional
		
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = msaaSampleCout;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = pipelineConfig.depthStencilConifg.depthTestEnable;
		depthStencil.depthWriteEnable = pipelineConfig.depthStencilConifg.depthWriteEnable;
		depthStencil.depthCompareOp = pipelineConfig.depthStencilConifg.depthCompareOp;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = pipelineConfig.depthStencilConifg.stencilTestEnable;
		depthStencil.front = pipelineConfig.depthStencilConifg.stencilFront; // Optional
		depthStencil.back = pipelineConfig.depthStencilConifg.stencilBack; // Optional

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments; 
		colorBlendAttachments.resize(pipelineConfig.colorBlendConfig.colorAttachmentConfig.size());
		for (int index = 0; index < colorBlendAttachments.size(); ++index)
		{
			colorBlendAttachments[index].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachments[index].blendEnable = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].blendEnable;
			colorBlendAttachments[index].srcColorBlendFactor = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].srcColorBlendFactor; // Optional
			colorBlendAttachments[index].dstColorBlendFactor = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].dstColorBlendFactor; // Optional
			colorBlendAttachments[index].colorBlendOp = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].colorBlendOp; // Optional
			colorBlendAttachments[index].srcAlphaBlendFactor = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].srcAlphaBlendFactor; // Optional
			colorBlendAttachments[index].dstAlphaBlendFactor = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].dstAlphaBlendFactor; // Optional
			colorBlendAttachments[index].alphaBlendOp = pipelineConfig.colorBlendConfig.colorAttachmentConfig[index].alphaBlendOp; // Optional
		}

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = colorBlendAttachments.size();
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = m_descriptorSetLayouts.size(); // Optional
		pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(
				RenderBackend::GetInstance().GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState; // Optional
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subpssIndex;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		if (vkCreateGraphicsPipelines(
				RenderBackend::GetInstance().GetDevice(), 
				VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}

	Pipeline::~Pipeline()
	{
		Destroy();
	}

	Pipeline::Pipeline(Pipeline&& other)
	{
		m_pipeline = other.m_pipeline;
		m_vertexShader = other.m_vertexShader;
		m_fragShader = other.m_fragShader;
		// m_descriptorSets = other.m_descriptorSets;
		m_descriptorSetLayouts = other.m_descriptorSetLayouts;
		m_pipelineLayout = other.m_pipelineLayout;
		m_bindDatas = other.m_bindDatas;
		m_currentWriteSets = other.m_currentWriteSets;

		other.m_pipeline = VK_NULL_HANDLE;
		other.m_vertexShader = VK_NULL_HANDLE;
		other.m_fragShader = VK_NULL_HANDLE;
		// other.m_descriptorSets.clear();
		other.m_descriptorSetLayouts.clear();
		other.m_pipelineLayout = VK_NULL_HANDLE;
		other.m_bindDatas.clear();
		other.m_currentWriteSets.clear();
	}

	Pipeline& Pipeline::operator=(Pipeline&& other)
	{
		// TODO: 在此处插入 return 语句
		Destroy();

		m_pipeline = other.m_pipeline;
		m_vertexShader = other.m_vertexShader;
		m_fragShader = other.m_fragShader;
		// m_descriptorSets = other.m_descriptorSets;
		m_descriptorSetLayouts = other.m_descriptorSetLayouts;
		m_pipelineLayout = other.m_pipelineLayout;
		m_bindDatas = other.m_bindDatas;
		m_currentWriteSets = other.m_currentWriteSets;

		other.m_pipeline = VK_NULL_HANDLE;
		other.m_vertexShader = VK_NULL_HANDLE;
		other.m_fragShader = VK_NULL_HANDLE;
		// other.m_descriptorSets.clear();
		other.m_descriptorSetLayouts.clear();
		other.m_pipelineLayout = VK_NULL_HANDLE;
		other.m_bindDatas.clear();
		other.m_currentWriteSets.clear();

		return *this;
	}

	void Pipeline::CreateShader(std::string const& vert_spir_path, std::string const& frag_spir_path)
	{
		auto vertShaderCode = readFile(vert_spir_path);
		auto fragShaderCode = readFile(frag_spir_path);
		m_vertexShader = CreateShaderModule(vertShaderCode);
		m_fragShader = CreateShaderModule(fragShaderCode);

		// 找个shader测试一下看看
		CollectSpirvMetaData(vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
		CollectSpirvMetaData(fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);

		GenerateVulkanDescriptorSetLayout();
	}

	void Pipeline::GenerateVulkanDescriptorSetLayout()
	{
		std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos;
		std::vector<VkDescriptorSetLayoutBinding>    layoutBindings;

		std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_setGroups;

		for (const auto& [resourceName, metaData] : m_bindDatas) {
			VkDescriptorSetLayoutBinding binding;
			VkShaderStageFlags stageFlags = metaData.shaderStageFlag;
			uint32_t set = metaData.set;

			VkDescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = metaData.binding;
			layoutBinding.descriptorType = metaData.descriptorType;
			layoutBinding.descriptorCount = metaData.count; // UBO Array
			layoutBinding.stageFlags = stageFlags;
			layoutBinding.pImmutableSamplers = nullptr; // Optional

			layoutBindings.push_back(layoutBinding);
			m_setGroups[set].push_back(layoutBinding);
		}

		for (const auto& [setIndex, bindingVecs] : m_setGroups) {
			VkDescriptorSetLayout setLayout;
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.bindingCount = bindingVecs.size();
			descriptorSetLayoutCreateInfo.pBindings = bindingVecs.data();
			if (vkCreateDescriptorSetLayout(
					RenderBackend::GetInstance().GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, &setLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}
			m_descriptorSetLayouts.push_back(setLayout);
			// for (auto frameIdex = 0; frameIdex < Config::MAX_FRAMES_IN_FLIGHT; ++frameIdex)
			// {
			// 	m_descriptorSets[frameIdex][setIndex] = (RenderBackend::GetInstance().GetDescriptorAllocator()->Allocate(setLayout));
			// }
		}
	}

	void Pipeline::CollectSpirvMetaData(std::vector<char> spivrBinary, VkShaderStageFlags shaderFlags)
	{
		const auto& device = RenderBackend::GetInstance().GetDevice();

		spirv_cross::CompilerGLSL    compiler(reinterpret_cast<const uint32_t*>(spivrBinary.data()), spivrBinary.size() / sizeof(uint32_t));
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		auto collectResource = [&](auto resource, VkDescriptorType descriptorType) {
			if (m_bindDatas.find(resource.name) == m_bindDatas.end()) {
				std::string_view resourceName = resource.name;
				uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				const spirv_cross::SPIRType& type = compiler.get_type(resource.type_id);
				uint32_t typeArraySize = type.array.size();
				uint32_t count = typeArraySize == 0 ? 1 : type.array[0];
				BindMetaData metaData{ set, binding, count, descriptorType, shaderFlags };
				m_bindDatas[resource.name] = metaData;
			}
			else {
				m_bindDatas[resource.name].shaderStageFlag |= shaderFlags;
			}
			};
		
		for (auto& resource : resources.uniform_buffers) {
			collectResource(resource, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		}

		for (auto& resource : resources.sampled_images) {
			collectResource(resource, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		}

		for (auto& resource : resources.separate_samplers) {
			collectResource(resource, VK_DESCRIPTOR_TYPE_SAMPLER);
		}

		for (auto& resource : resources.separate_images) {
			collectResource(resource, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		}

		for (auto& resource : resources.subpass_inputs) {
			collectResource(resource, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
		}
	}
}