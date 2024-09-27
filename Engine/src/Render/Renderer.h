#pragma once
#include <string>
#include <map>

#include "../RHI/RenderPass.h"
#include "Scene.h"

namespace VulkanEngine
{
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

		virtual void Render(Scene& scene) = 0;

	protected:
		virtual void Init() = 0; // 不同Renderer自定义，添加RenderPass，每一个RenderPass还得负责创建自己FrameBuffer需要用到的Texture
		void AddRenderPass(RenderPassEnum renderPass);
	protected:
		std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
	};
}