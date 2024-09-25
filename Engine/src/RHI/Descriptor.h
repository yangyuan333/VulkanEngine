#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.h"

namespace VulkanEngine {

	struct PoolSizes { std::vector<VkDescriptorPoolSize> sizes; };

	class DescriptorLayoutCache 
	{
	public:
		struct DescriptorLayoutInfo {
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			bool operator==(const DescriptorLayoutInfo& other) const;
			size_t hash() const;
		};
		VkDescriptorSetLayout CreateDescriptorsetlayout(const VkDescriptorSetLayoutCreateInfo layoutInfo);

		inline void Init(VkDevice device) { m_device = device; }
		void CleanUp();

	private:
		struct DescriptorLayoutHash {
			std::size_t operator()(const DescriptorLayoutInfo& k) const { return k.hash(); }
		};
		using LayoutCache =
			std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash>;

		VkDevice m_device;
		LayoutCache m_layoutCache;
	};

	class DescriptorAllocator 
	{
	public:
		void Init(VkDevice device);
		void CleanUp();

		VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);

	private:
		VkDevice m_device;
		VkDescriptorPool m_currentPool{ nullptr };
	};

	/// <summary>
	/// ����ֻ���� descriptorSet �� decriptorLayout��
	/// ����� buffer texture ��Դ�󶨣���ÿ�� GameObject �Ǳ�����
	/// </summary>
	class DescriptorBuilder
	{
	public:
		static DescriptorBuilder Begin(DescriptorLayoutCache* cache, DescriptorAllocator* allocator);
		static DescriptorBuilder Begin();

		DescriptorBuilder& BindDescritptor(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);

		bool Build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
		bool Build(VkDescriptorSet& set);

	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;

		std::shared_ptr<DescriptorLayoutCache> m_cache;
		std::shared_ptr<DescriptorAllocator> m_alloc;
	};

} // namespace wind