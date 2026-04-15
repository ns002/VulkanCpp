#pragma once
#include "pch.h"

namespace VWrapper
{
	struct QueueFamilyIndices;
	struct SwapChainSupportDetails;

	class vLogicalDevice
	{
		friend class vDeviceHandler;

	public:
		vLogicalDevice(const VkInstance& r_instance);
		~vLogicalDevice() = default;

		const VkQueue& GetGraphicsQueue()		const { return graphicsQueue; }
		const VkQueue& GetPresentQueue()		const { return presentQueue; }
		const VkDevice& GetLogicalDevicePtr()	const { return device; }

	private:
		const VkInstance& instance;
		VkDevice device = nullptr;
		VkQueue graphicsQueue = nullptr;
		VkQueue presentQueue = nullptr;

		void CreateLogicalDevice(QueueFamilyIndices indices, const VkPhysicalDevice& PD);
	};
}