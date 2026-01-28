#pragma once
#include "pch.h"

class Window;

namespace VWrapper
{
	class vValidationCore;
	class RenderPipeline;

	class VulkanCore
	{
		friend class vValidationCore;

	public:
		//VulkanCore will hook itself to this window class (and this should be considered as the MAIN window)

		static void Init(std::shared_ptr<Window> p_window);
		static void Clean();

		static std::shared_ptr<const Window> GetWindow() { return window; }
		static std::shared_ptr<const RenderPipeline> GetRenderer() { return pipeline; }

	private:

		static bool CoreInitialized;
		static std::shared_ptr<Window> window;		//we should in control here, shared to anything else from here
		static VkInstance instance;
		static vValidationCore* validationLayer;
		static std::shared_ptr<RenderPipeline> pipeline;

		static void CreateInstanceAndValidator();
		static VkApplicationInfo FillAppInfo();
		static std::vector<const char*> getRequiredExtensions();
	};
}