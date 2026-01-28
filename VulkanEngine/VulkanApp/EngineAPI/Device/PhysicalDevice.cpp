#include "pch.h"
#include "../../Window.h"
#include "../Extensions&Layers.inl"
#include "PhysicalDevice.h"
#include "SupportStructure.h"


namespace VWrapper
{
	vPhysicalDevice::vPhysicalDevice(const VkInstance& r_instance, std::shared_ptr<Window> r_window, std::optional<SwapChainSupportDetails>& r_swapChainSupport, QueueFamilyIndices& r_indices) :
		instance(r_instance), window(r_window), swapChainSupport(r_swapChainSupport), indices(r_indices)
	{

	}

	void vPhysicalDevice::PickPhysicalDevice()
	{
		const unsigned char arr_length = 16U;
		VkPhysicalDevice physicalDevices[arr_length];	//cannot imagine you will ever need more
		uint32_t deviceCount = 0;

		//Querry machine
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0) throw std::runtime_error("failed to find devices with Vulkan support!");
		else if (deviceCount > arr_length) throw std::runtime_error("Found too many physical devices. would result in array overflow!");
		vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices);
		if (physicalDevices[0] == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU! (#1)");

		device = FindBestSuitable(physicalDevices, deviceCount).second;	//first is the rating of the device (if you need it)

		if (!FindQueueFamilies()) throw std::runtime_error("The automatically chosen GPU (physical device) does not support 'graphics-queue' or 'swap-chain' extensions.");
	}

	const std::pair<uint16_t, VkPhysicalDevice> vPhysicalDevice::FindBestSuitable(VkPhysicalDevice* PDs, const uint32_t& size)
	{
		std::map<uint16_t, VkPhysicalDevice> ratedDevices;	//first find all devices
		for (uint32_t i = 0; i < size; ++i)
		{
			//Querry device
			VkPhysicalDeviceFeatures deviceFeatures; vkGetPhysicalDeviceFeatures(PDs[i], &deviceFeatures);
			if (!deviceFeatures.geometryShader)
			{
				ratedDevices.insert(std::make_pair((uint16_t)(0U), PDs[i]));	// Application can't function without geometry shaders
				continue;	//skip for loop itteration
			}

			//Rate the device
			VkPhysicalDeviceProperties deviceProperties; uint16_t gpuRating = 0;	//made up rating system
			vkGetPhysicalDeviceProperties(PDs[i], &deviceProperties);
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) gpuRating += 30000;	// Discrete GPUs have a significant performance advantage
			gpuRating += (uint16_t)deviceProperties.limits.maxImageDimension2D;	// Maximum possible size of textures affects graphics quality
			ratedDevices.insert(std::make_pair(gpuRating, PDs[i]));
		}
		if (ratedDevices.size() == 0U) throw std::runtime_error("failed to find a suitable GPU! (#2)");

		//Find the best rated device
		std::pair<uint16_t, VkPhysicalDevice> bestDevice = std::make_pair((uint16_t)(0U), VK_NULL_HANDLE); //zero init
		for (const auto& device : ratedDevices) if (device.first > bestDevice.first) bestDevice = device;

		if (bestDevice.second == VK_NULL_HANDLE || bestDevice.first == 0U) throw std::runtime_error("failed to find a suitable GPU! (#3)");	//check if not zero
		std::string info("Graphical processor rated at: "); info.append(std::to_string(bestDevice.first)); info.append(1, '\n');
		DebugPrint(info); return bestDevice;
	}

	bool vPhysicalDevice::FindQueueFamilies()
	{
		const VkSurfaceKHR& surface = window->GetSurface();
		if (surface == VK_NULL_HANDLE) throw std::runtime_error("VK Surface was null, could not validate presentFamily");

		// Logic to find queue family indices to populate struct with
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		if (queueFamilyCount == 0) throw std::runtime_error("No device queue was found");
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (size_t i = 0; i < queueFamilies.size(); ++i)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = static_cast<uint32_t>(i);

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<uint32_t>(i), surface, &presentSupport);
			if (presentSupport) indices.presentFamily = static_cast<uint32_t>(i);

			bool swapChainAdequate = false, extensionSupport = true;
			if (!swapChainSupport.has_value()) extensionSupport = CheckDeviceExtensionSupport();	//step is skipped if swapChainSupport has a value (we should have already checked this)
			if (extensionSupport)
			{
				swapChainSupport = querySwapChainSupport();
				swapChainAdequate = !(swapChainSupport.value().formats.empty() || swapChainSupport.value().presentModes.empty());
			}

			if (indices.isComplete() && swapChainAdequate) return true;
		}
		return false;
	}

	//checks if all required device extensions are available
	//this is true if the remaining set is empty
	bool vPhysicalDevice::CheckDeviceExtensionSupport()
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);
		return requiredExtensions.empty();
	}

	const SwapChainSupportDetails vPhysicalDevice::querySwapChainSupport() const
	{
		const VkSurfaceKHR& surface = window->GetSurface();

		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}