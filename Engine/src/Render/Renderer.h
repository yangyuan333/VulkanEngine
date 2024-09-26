#pragma once
#include "../RHI/RenderPass.h"
#include <string>
#include <map>
#include "../Utility/Config.h"
#include "Scene.h"

namespace VulkanEngine
{
	class Renderer
	{
	public:
		Renderer();
		virtual ~Renderer();

		virtual void Render(Scene& scene) = 0;

	protected:
		virtual void Init() = 0; // ��ͬRenderer�Զ��壬���RenderPass��ÿһ��RenderPass���ø��𴴽��Լ�FrameBuffer��Ҫ�õ���Texture
		void AddRenderPass(RenderPassEnum renderPass);
	protected:
		std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
	};
}