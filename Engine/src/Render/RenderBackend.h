#pragma once

#include "../Utility/SingleTon.h"
#include "../RHI/VirtualFrame.h"
#include "../RHI/Descriptor.h"
#include "../RHI/Image.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace VulkanEngine
{
	/// <summary>
	/// Vulkan Instance
	/// Multiple Render API Prepare---now only for vulkan
	/// </summary>
	
	struct QueueFamilyIndices
	{
		int graphicsFamily = -1;
		int presentFamily = -1;
		bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class RenderBackend : public Singleton<RenderBackend>
	{
		friend class Singleton<RenderBackend>;
	public:
		// 构造函数初始化需要；
		void CreateInstance();
		void SetupDebugCallback();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();
		void CreateDescriptorCacheAndAllocator();
		void CreateSwapChain();
	public:
		static void Init(); // 提供首次访问单例的通道
	public:
		// 放到 Init 里面，resize会使用；
		void CreateVirtualFrame();
	public:
		void RecreateSwapChain();
	public:
		std::shared_ptr<CommandBuffer> BeginSingleTimeCommand();
		void SubmitSingleTimeCommand(VkCommandBuffer cmdBuffer);
	public:
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSampleCountFlagBits getMaxUsableSampleCount();
		void cleanupSwapChain();
	public:
		inline VkCommandPool& GetCommandPool() { return m_commandPool; }
		inline VkDevice& GetDevice() { return m_device; }
		inline VkPhysicalDevice& GetPhyDevice() { return m_physicalDevice; }
		inline std::shared_ptr<DescriptorAllocator>& GetDescriptorAllocator() { return m_descriptorAllocator; }
		inline VkSampleCountFlagBits GetMsaaSampleBit() const { return m_msaaSamples; }
		inline VkSampleCountFlagBits GetMsaaSampleBit() { return m_msaaSamples; }
		inline VkSwapchainKHR& GetSwapChain() { return m_swapChain; }
		inline VkQueue& GetPresentQueue() { return m_presentQueue; }
		inline VkQueue& GetGraphicsQueue() { return m_graphicsQueue; }
		inline auto& GetStagingBuffer() { return m_virtualFrames.GetCurrentFrame().stagingBuffer; }
		inline auto GetCurrentFrameIndex() { return m_virtualFrames.GetCurrentFrameIndex(); }
		inline auto GetImageIndex() { return m_virtualFrames.GetImageIndex(); }
		inline auto& GetSwapChainImages() { return m_swapChainImages; }
		inline auto& GetSwapChainExtent() { return m_swapChainExtent; }
		inline auto& GetCurrentFrame() { return m_virtualFrames.GetCurrentFrame(); }
		inline auto& GetSampler() { return m_sampler; }
		void StartFrame();
		void EndFrame();
		void SetDynamicViewportScissor();
	public:
		~RenderBackend();
	private:
		RenderBackend();
	private:
		VkInstance m_vkInstance;
		VkDebugUtilsMessengerEXT callback;

		VkPhysicalDevice           m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkDevice                   m_device;

		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		// VkQueue m_computeQueue;

		VkCommandPool m_commandPool;
		VkCommandBuffer m_immediateCmdBuffer;
		VkFence m_immediateFence;

		VkSurfaceKHR m_surface;
		VkSwapchainKHR m_swapChain;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<std::shared_ptr<Image>> m_swapChainImages;

		VirtualFrameProvider m_virtualFrames;

		std::shared_ptr<DescriptorAllocator> m_descriptorAllocator;

		VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		std::shared_ptr<Sampler> m_sampler;
	};
}