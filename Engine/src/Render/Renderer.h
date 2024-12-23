#pragma once
#include <string>
#include <map>

#include "../RHI/RenderPass.h"
#include "Scene.h"

namespace VulkanEngine
{
	class FrameBuffer;

	enum class RenderPassEnum
	{
		PbrDeferredPass, // v1.0
		ShadowMapPass, // v1.1
		TaaPass, // v2.0
		BloomPass // v2.1
	};

	class Renderer
	{
	public:
		Renderer();
		virtual ~Renderer();
		Renderer(Renderer const& other) = delete;
		Renderer& operator=(Renderer const& other) = delete;

		virtual void Render(Scene& scene) = 0;
		virtual void CreateFrameBuffer() = 0;
		virtual void RecreateFrameBuffer() = 0;

		inline auto& GetRenderPass(RenderPassEnum passEnum) { return m_renderPasses[passEnum]; }

	protected:
		virtual void Init() = 0; // 不同Renderer自定义，添加RenderPass，每一个RenderPass还得负责创建自己FrameBuffer需要用到的Texture
		void AddRenderPass(RenderPassEnum renderPass);
	protected:
		std::map<RenderPassEnum, std::shared_ptr<RenderPass>> m_renderPasses;
		std::map<RenderPassEnum, std::vector<std::shared_ptr<FrameBuffer>>> m_frameBuffers; // 这里得确保只有这里用到了framebuffer，否则可能会影响到recreate销毁
	};
}