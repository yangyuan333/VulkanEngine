#pragma once

#include <memory>
#include <vector>

#include "../RHI/Pipeline.h"
#include "../RHI/RenderPass.h"

namespace VulkanEngine
{
	class Material
	{
	public:
		Material(MaterialType m_type, std::shared_ptr<RenderPass> renderpass);
		~Material();
		Material(Material const& material) = delete;
		Material& operator=(Material const& material) = delete;
	public:
		MaterialType GetMaterialType() const { return m_type; }
		MaterialType GetMaterialType() { return m_type; }
	private:
		std::shared_ptr<RenderPass> m_RenderPass;
		MaterialType m_type;
	};
}
