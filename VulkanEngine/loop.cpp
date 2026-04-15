#include "pch.h"
#include "VulkanApp/VulkanApp.h"
#include "VulkanApp/Engine/Core.h"
#include "VulkanApp/Engine/Rendering/RenderPipeline.h"

namespace VWrapper
{
	void App::Run()
	{
		assert(initialized);	//Initialize the app before Run()

		//Create a scope so that the window pointer is discarded before Clean() is called
		std::shared_ptr<const Window> window = VulkanCore::GetWindow();
		std::shared_ptr<const RenderPipeline> renderer = VulkanCore::GetRenderer();
		DebugPrint("Start loop.\n"); size_t frame = 0;

		while (!window->ShouldClose() && !exitRequest)
		{
			glfwPollEvents();

			//example
			PhysicsLoop(); EntityUpdate(); Animate();
			if (renderer->DrawProc(frame++)) window->Show();

		}

		window->Hide();  //gets rid of the window, but still need to destroy it
		DebugPrint("Exit loop.\n");
	}
}