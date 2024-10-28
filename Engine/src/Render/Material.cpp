#include "Material.h"
#include "GameObject.h"

namespace VulkanEngine
{
	Material::Material(MaterialType type, std::shared_ptr<RenderPass> renderpass)
	{
		// 这里最好加个匹配检测
		m_type = type;
		m_RenderPass = renderpass;
	}

	Material::~Material()
	{
		
	}

	void Material::BindGameObject(GameObject& object)
	{
		m_RenderPass->BindGameObject(object);
		// auto& descriptorSet = m_RenderPass->BindGameObject(object);
		// object->GetDescriptorSets()[m_type] = descriptorSet;
	}
}