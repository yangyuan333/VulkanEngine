#pragma once
#include "Renderer.h"
#include <map>

namespace VulkanEngine
{
	class Image;
	class DeferredRenderer : public Renderer
	{
	public:
		enum class ImageEnum
		{
			Depth,
			GBufferA,
			GBufferB,
			GBufferC,
			GBufferD
		};
	public:
		DeferredRenderer();
		~DeferredRenderer() override;
		DeferredRenderer(DeferredRenderer const& other) = delete;
		DeferredRenderer& operator=(DeferredRenderer const& other) = delete;

		void CreatePassImageViews();

		virtual void Render(Scene& scene) override;
		virtual void CreateFrameBuffer() override;
		virtual void RecreateFrameBuffer() override;

	protected:
		virtual void Init() override;

	protected:
		std::map<ImageEnum, std::shared_ptr<Image>> m_images;
	};

}