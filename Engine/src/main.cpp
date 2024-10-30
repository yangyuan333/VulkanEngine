#include "Engine/Engine.h"
#include <memory>
#include "renderdoc_app.h"
#include <Windows.h>

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

	// RENDERDOC_API_1_1_2* rdoc_api = nullptr;//API½Ó¿Ú
	
#if DEBUG_RENDERDOC
	RENDERDOC_API_1_1_2*& rdoc_api = VulkanEngine::Engine::GetInstance().rdoc_api;
	if (HMODULE mod = LoadLibraryA("C:\\Program Files\\RenderDoc\\renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
		assert(ret == 1);
		rdoc_api->SetCaptureFilePathTemplate("D://GraphicsLearning//Code//VulkanEngine//rdc");
	}
	else
	{
		std::cout << "RenderDoc Lode Error: " << GetLastError() << std::endl;
	}
	// rdoc_api->TriggerCapture();
	// if (rdoc_api->IsTargetControlConnected())
	// {
	// 	rdoc_api->ShowReplayUI();
	// }
	// else
	// {
	// 	rdoc_api->LaunchReplayUI(1, nullptr);
	// }
#endif

	VulkanEngine::Engine::GetInstance().Init();
	VulkanEngine::Engine::GetInstance().Run();
}