#pragma once
#include "pch.h"
#include "SupportStructure.h"

class Window;

//TODO: add status enum or bool for logical and physical device to check their status... eg:active, destroyed
//TODO: let deviceHandler create the swapchain and image view
namespace VWrapper
{
	class vSwapChain;
	class vPhysicalDevice;
	class vLogicalDevice;

	class vDeviceHandler
	{
		vDeviceHandler() = delete;
		vDeviceHandler(vDeviceHandler&) = delete;

		static void CreateDevices();
		static void DestroyLogicalDevice();

		static std::optional<SwapChainSupportDetails> swapChainSupport; 	//do not acces this before PickPhysicalDevice() is called for first time!
		static std::unique_ptr<vPhysicalDevice> physicalDevice;
		static std::unique_ptr<vLogicalDevice> logicalDevice;
		static QueueFamilyIndices indices;

	public:

		static void InitDevices(const VkInstance& r_instance, std::shared_ptr<Window> r_window);
		static void Clean();

		static vSwapChain* CreateSwapChain(std::shared_ptr<Window> p_window);

		//Gets the class variables in this manager class
		static const vPhysicalDevice& GetPhysicalDeviceClass() { return *physicalDevice; };
		//Gets the class variables in this manager class
		static const vLogicalDevice& GetLogicalDeviceClass() { return *logicalDevice; };

		//looks into the class variables and return VkDevice instance
		static const VkPhysicalDevice& GetPhysicalDevicePtr();
		//looks into the class variables and return VkDevice instance
		static const VkDevice& GetLogicalDevicePtr();

		static const SwapChainSupportDetails& GetSwapChainSupportDetails();
		static const QueueFamilyIndices& GetQueueFamilyIndices() { return indices; };
	};
}