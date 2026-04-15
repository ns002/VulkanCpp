#include "pch.h"
#include "../Window.h"
#include "Core.h"
#include "Device/DeviceHandler.h"
#include "Rendering/RenderPipeline.h"
#include "Shader/VShaderCompiler.h"
#include "Shader/ShaderHandler.h"
#ifdef DEBUG_TOOLS
#include "Validation.h"
#endif

namespace VWrapper
{
	bool VulkanCore::CoreInitialized = false;
	std::shared_ptr<Window> VulkanCore::window{};
	VkInstance VulkanCore::instance = VK_NULL_HANDLE;
	vValidationCore* VulkanCore::validationLayer = nullptr;
	std::shared_ptr<RenderPipeline> VulkanCore::pipeline{};

	//watch out as initialization order is rather important here
	void VulkanCore::Init(std::shared_ptr<Window> p_window)
	{
		window = p_window;
#ifdef DEBUG_TOOLS
		validationLayer = new vValidationCore(instance);	//checks if vulkan is included correctly, and additionally if system supports necessary extensions
#endif
		CreateInstanceAndValidator();	//don't use instance before this call!
		CoreInitialized = true;
		window->CreateVkSurface(instance);
		vDeviceHandler::InitDevices(instance, window);

		DebugPrint("VK_CORE - initialized\n");

		ShaderCompiler::Compile();	//engine compiles shaders on the go, using a few bat scripts.
		if (!(ShaderHandler::AppendShader(ShaderType::VERTEX) && ShaderHandler::AppendShader(ShaderType::FRAGMENT)))
			throw std::runtime_error("Failed to emplace into shader_map structre");
		ShaderHandler::ShaderStageInfo();
		pipeline = std::make_shared<RenderPipeline>(vDeviceHandler::CreateSwapChain(window));

		DebugPrint("VK: Render pipeline initialized\n\n");
	}

	void VulkanCore::Clean()
	{
		if (CoreInitialized == true)
		{
			vkDeviceWaitIdle(vDeviceHandler::GetLogicalDevicePtr());

			if (pipeline != nullptr) pipeline->Clean();	//will call waitForFences first
			ShaderHandler::Clean();
			vDeviceHandler::Clean();
			window->DestroyWindow(instance);
			if (validationLayer != nullptr) delete validationLayer;

			vkDestroyInstance(instance, nullptr);	//very last vulkan call
		}
		DebugPrint("VK_CORE - exit signal\n");
	}

	void VulkanCore::CreateInstanceAndValidator()		//initiating the validator is a little intertwined with the instance :(
	{
		VkApplicationInfo appInfo = FillAppInfo(); VkInstanceCreateInfo createInfo = {};
		std::vector<const char*> extensions = getRequiredExtensions();
		createInfo.pApplicationInfo = &appInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

#ifdef DEBUG_TOOLS
		auto messengerCreateInfo = vValidationCore::FillMessengerCreateInfo(&createInfo);	//instance needs messenger info if this will be used
#endif

		VkResult err = vkCreateInstance(&createInfo, nullptr, &instance);
		if (err != VK_SUCCESS) throw std::runtime_error("failed to create instance!");

#ifdef DEBUG_TOOLS
		validationLayer->Init(messengerCreateInfo);			//preferably this is called right after creating the instance
#endif
	}

	VkApplicationInfo VulkanCore::FillAppInfo()
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "V-Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "V-Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		return appInfo;
	}

	std::vector<const char*> VulkanCore::getRequiredExtensions()
	{
		// Get GLFW required extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);			//glfw couldn't move this data into a vec??? smh
		if (buildConfig != BuildConfiguration::Distro) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

}