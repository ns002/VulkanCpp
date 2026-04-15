#include "pch.h"
#include "../Extensions&Layers.inl"
#include "LogicalDevice.h"
#include "SupportStructure.h"

namespace VWrapper
{
	vLogicalDevice::vLogicalDevice(const VkInstance& r_instance) :
		instance(r_instance)
	{

	}

	void vLogicalDevice::CreateLogicalDevice(QueueFamilyIndices indices, const VkPhysicalDevice& PD)
	{
		//create info for graphics_queue
		float queuePriority = 1.0f; VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//defined for later use
		VkPhysicalDeviceFeatures deviceFeatures{};

		//create info for the logical device
		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		logicalDeviceCreateInfo.queueCreateInfoCount = 1;
		logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = 0;

		//This is apparently so we can support old versions of Vulkan
#ifdef DEBUG_TOOLS
		logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif

		if (vkCreateDevice(PD, &logicalDeviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
}