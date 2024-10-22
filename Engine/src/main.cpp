#include "Engine/Engine.h"
#include <memory>

/*
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
*/

int main()
{
	// glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1, 0, 0));
	// rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.0f), glm::vec3(0, 1, 0));
	// rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.0f), glm::vec3(0, 0, 1));
	// std::cout << rotationMatrix << std::endl;
	// CheckPipelineShader();

	VulkanEngine::Engine::GetInstance().Init();
	VulkanEngine::Engine::GetInstance().Run();
}