#include "pch.h"
#include "../Rendering/SwapChain.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "DeviceHandler.h"

namespace VWrapper
{
	std::unique_ptr<vLogicalDevice> vDeviceHandler::logicalDevice = nullptr;
	std::unique_ptr<vPhysicalDevice> vDeviceHandler::physicalDevice = nullptr;
	std::optional<SwapChainSupportDetails> vDeviceHandler::swapChainSupport = std::nullopt;		//do not acces this before PickPhysicalDevice() is called for first time!
	QueueFamilyIndices vDeviceHandler::indices;


	void vDeviceHandler::CreateDevices()
	{
		physicalDevice->PickPhysicalDevice();
		logicalDevice->CreateLogicalDevice(indices, GetPhysicalDevicePtr());
	}

	void vDeviceHandler::DestroyLogicalDevice()
	{
		if (logicalDevice->device != VK_NULL_HANDLE) vkDestroyDevice(logicalDevice->device, nullptr);
	}
	
	void vDeviceHandler::InitDevices(const VkInstance& r_instance, std::shared_ptr<Window> p_window)
	{
		//supplying logical and physical device with references to the VkInstance and the window
		physicalDevice = std::make_unique<vPhysicalDevice>(r_instance, p_window, swapChainSupport, indices);
		logicalDevice = std::make_unique<vLogicalDevice>(r_instance);
		CreateDevices();	//init here
	}
	void vDeviceHandler::Clean()
	{
		DestroyLogicalDevice();
	}
	
	vSwapChain* vDeviceHandler::CreateSwapChain(std::shared_ptr<Window> p_window)
	{
		return new vSwapChain(GetLogicalDevicePtr(), p_window, indices, swapChainSupport);
	}

	const VkPhysicalDevice& vDeviceHandler::GetPhysicalDevicePtr()
	{ 
		return physicalDevice->device; 
	}
	const VkDevice& vDeviceHandler::GetLogicalDevicePtr() 
	{ 
		return logicalDevice->device;
	}
	const SwapChainSupportDetails& vDeviceHandler::GetSwapChainSupportDetails()
	{
		if (swapChainSupport.has_value())
			return swapChainSupport.value();
		throw std::runtime_error("SwapChainSupportDetails struct requested, but does not exist");
	}
}