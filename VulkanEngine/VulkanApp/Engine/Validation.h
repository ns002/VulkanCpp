#pragma once
#include "pch.h"

namespace VWrapper
{
	class vValidationCore
	{
		friend class VulkanCore;

	public:

		vValidationCore(const VkInstance& rInstance);
		~vValidationCore();

		void Init(std::shared_ptr<VkDebugUtilsMessengerCreateInfoEXT> createInfo);

	private:
		VkDebugUtilsMessengerEXT messenger = nullptr;

		const VkInstance& instance;

		static void ValidateVkExtensions();
		static void CheckValidationLayerSupport();
		static std::shared_ptr<VkDebugUtilsMessengerCreateInfoEXT> FillMessengerCreateInfo(VkInstanceCreateInfo* instanceCreateInfo = nullptr);

		static VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);
		static void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator
		);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		) noexcept;
	};
}