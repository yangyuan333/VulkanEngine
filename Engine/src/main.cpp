#include "Resource/ModelLoader.h"
#include "Engine/Engine.h"
#include "Render/PbrDeferredPass.h"
#include <memory>

void CheckModelLoader()
{
	std::string modelPathDragon = "D:\\GraphicsLearning\\Code\\VulkanAbstractionLayer\\examples\\models\\dragon\\dragon.obj";
	VulkanEngine::ModelData modelDataDragon = VulkanEngine::ModelLoader::Load(modelPathDragon);
	std::string modelPathSponza = "D:\\GraphicsLearning\\Code\\VulkanAbstractionLayer\\examples\\models\\Sponza\\glTF\\Sponza.gltf";
	VulkanEngine::ModelData modelDataSponza = VulkanEngine::ModelLoader::Load(modelPathSponza);
}

void CheckPipelineShader()
{
	 VulkanEngine::Engine::GetInstance().Init();
	 std::shared_ptr<VulkanEngine::RenderPass> renderPass = std::make_shared<VulkanEngine::PbrDeferredPass>();
	 renderPass->Build();
}

int main()
{
	CheckPipelineShader();
}