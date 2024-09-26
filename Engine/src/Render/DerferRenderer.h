#pragma once
#include "Renderer.h"

namespace VulkanEngine
{
	class DeferredRenderer : public Renderer
	{
	public:
		DeferredRenderer() { Init(); }
		~DeferredRenderer();

		virtual void Render(Scene& scene) override;

	protected:
		virtual void Init() override;
	};

}