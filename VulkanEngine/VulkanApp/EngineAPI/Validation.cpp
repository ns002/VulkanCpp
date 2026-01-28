#include "pch.h"
#include "Extensions&Layers.inl"
#include "Validation.h"

#include <sstream>

//validation layer will not be used in DISTRO mode
//be aware that errors will occur without detailed prints in vulkan funtions

namespace VWrapper
{
	void vValidationCore::ValidateVkExtensions()
	{
		uint32_t extensionCount = 0; std::vector<std::string> text;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		for (const auto& extension : extensions) 
			text.push_back(std::string("Available extension: ") + extension.extensionName + '\n');

		if (text.size()) DebugPrint(text.data(), text.size(), std::nullopt, std::nullopt);
		else throw std::runtime_error("No available Vulkan extensions!");
	}

	void vValidationCore::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		
		
		std::vector<std::string> missingLayers, okLayers;

		//Find out if validation layers are supported on this system 
		//If any required layer is unsupported prints info and throws
		for (const char* name : validationLayers)
		{
			bool layerFound = false;
			for (const auto& checked : availableLayers)
				if (CStringsEqual(name, checked.layerName))
				{
					okLayers.push_back(std::string("Layer Validated: ") + name + '\n');
					layerFound = true; break;
				}
			if (!layerFound) missingLayers.push_back(std::string("Validation Layer not found!: \"") + name + "\"\n");
		}

		if (missingLayers.size())
		{
			DebugPrint(missingLayers.data(), missingLayers.size(), std::nullopt, std::nullopt);
			throw std::runtime_error("Validation layers requested, but not available.");
		}
		else DebugPrint(okLayers.data(), okLayers.size(), std::nullopt, '\n');
	}


	vValidationCore::vValidationCore(const VkInstance& rInstance) : instance(rInstance)
	{
		ValidateVkExtensions();
		CheckValidationLayerSupport();
	}

	vValidationCore::~vValidationCore()
	{
		DestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
	}

	void vValidationCore::Init(std::shared_ptr<VkDebugUtilsMessengerCreateInfoEXT> createInfo)
	{
		VkResult err = CreateDebugUtilsMessengerEXT(instance, createInfo.get(), nullptr, &messenger);
		if (err != VK_SUCCESS) throw std::runtime_error("Failed to set up debug messenger!");
		DebugPrint("Validator process started.\n");
	}

	//Be SURE to call delete on the returned pointer after its done being used!
	std::shared_ptr<VkDebugUtilsMessengerCreateInfoEXT> vValidationCore::FillMessengerCreateInfo(VkInstanceCreateInfo* instanceCreateInfo)
	{
		std::shared_ptr<VkDebugUtilsMessengerCreateInfoEXT> createInfo(std::make_shared<VkDebugUtilsMessengerCreateInfoEXT>());
		createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo->messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |*//* VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |*/ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo->pfnUserCallback = debugCallback;
		createInfo->pUserData = nullptr; // Optional

		//if we are creating the vulkan instance, and call this function. we can add some extra details here
		//this is only done here to clarify that this is part of the validation proces
		if (instanceCreateInfo != nullptr) {
			instanceCreateInfo->enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			instanceCreateInfo->ppEnabledLayerNames = validationLayers.data();
			instanceCreateInfo->pNext = createInfo.get();
		}
		
		return createInfo;
	}

	VkResult vValidationCore::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void vValidationCore::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func != nullptr) func(instance, debugMessenger, pAllocator);
	}

	static uint32_t CHINA = 1;	//bruh fuck OFFF
	static uint32_t count = 1;
	VKAPI_ATTR VkBool32 VKAPI_CALL vValidationCore::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
	{
		if (std::string(pCallbackData->pMessage).rfind("EOSOverlayVkLayer") != std::string::npos) { ++CHINA; return VK_TRUE; }	//FUCK OFFF!!! stop clogging it

		const char* severity;
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	severity = " WARN";	 break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		severity = " ERR";	 break;

			default: severity = ""; break;
		}

		std::stringstream temp; temp << "VALIDATION MESSAGE (" << count++ << ')' << severity << ":\n" << pCallbackData->pMessage << "\n\n";
		DebugPrint(temp.str()); return VK_FALSE;
	}
}