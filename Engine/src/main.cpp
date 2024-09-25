#include "Resource/ModelLoader.h"

int main()
{
	std::string modelPathDragon = "D:\\GraphicsLearning\\Code\\VulkanAbstractionLayer\\examples\\models\\dragon\\dragon.obj";
	VulkanEngine::ModelData modelDataDragon = VulkanEngine::ModelLoader::Load(modelPathDragon);

	std::string modelPathSponza = "D:\\GraphicsLearning\\Code\\VulkanAbstractionLayer\\examples\\models\\Sponza\\glTF\\Sponza.gltf";
	VulkanEngine::ModelData modelDataSponza = VulkanEngine::ModelLoader::Load(modelPathSponza);
}