#pragma once
#include "pch.h"

class Window;

namespace VWrapper
{
	struct QueueFamilyIndices;
	struct SwapChainSupportDetails;

	class vPhysicalDevice
	{
		friend class vDeviceHandler;

	public:
		vPhysicalDevice(
			const VkInstance& r_instance,
			std::shared_ptr<Window> p_window,
			std::optional<SwapChainSupportDetails>& r_swapChainSupport,
			QueueFamilyIndices& r_indices);

		~vPhysicalDevice() = default;

		const SwapChainSupportDetails querySwapChainSupport() const;

	private:		

		const VkInstance& instance;
		std::shared_ptr<Window> window;
		VkPhysicalDevice device = nullptr;

		std::optional<SwapChainSupportDetails>& swapChainSupport;
		QueueFamilyIndices& indices;

		void PickPhysicalDevice();
		static const std::pair<uint16_t, VkPhysicalDevice> FindBestSuitable(VkPhysicalDevice* PDs, const uint32_t& size);
		bool FindQueueFamilies();
		bool CheckDeviceExtensionSupport();
		
	};
}