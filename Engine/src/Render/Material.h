#pragma once

#include <memory>
#include <vector>

#include "../RHI/Pipeline.h"
#include "../RHI/RenderPass.h"

namespace VulkanEngine
{
	class GameObject;
	class Material
	{
	public:
		Material(MaterialType type, std::shared_ptr<RenderPass> renderpass);
		~Material();
		Material(Material const& material) = delete;
		Material& operator=(Material const& material) = delete;
	public:
		MaterialType GetMaterialType() const { return m_type; }
		MaterialType GetMaterialType() { return m_type; }
		std::vector<std::shared_ptr<Pipeline>>& GetPipelines() { return m_RenderPass->GetPipelines(); }
		void BindGameObject(std::shared_ptr<GameObject> object);
		inline auto GetRenderpass() { return m_RenderPass; }
	private:
		std::shared_ptr<RenderPass> m_RenderPass;
		MaterialType m_type;
	};
}
