#include "pch.h"
#include "../../Window.h"
#include "SwapChain.h"

namespace VWrapper 
{
	vSwapChain::vSwapChain(const VkDevice& logicalDevice, std::shared_ptr<Window> window, const QueueFamilyIndices& indices, std::optional<SwapChainSupportDetails>& swapChainSupport) :
		LD(logicalDevice), swapChainDetails(swapChainSupport.value())
	{
		CreateSwapChain(window, indices);
		CreateImageViews();
	}

	vSwapChain::~vSwapChain()
	{
		for (VkFramebuffer buf : framebuffers)
			vkDestroyFramebuffer(LD, buf, nullptr);
		for (VkImageView view : imageViews)
			vkDestroyImageView(LD, view, nullptr);
		vkDestroySwapchainKHR(LD, swapChain, nullptr);
	}


	void vSwapChain::CreateSwapChain(std::shared_ptr<const Window> window, const QueueFamilyIndices& indices)
	{
		swapChainDetails.surfaceFormat = chooseSwapSurfaceFormat();
		swapChainDetails.swapChainImageFormat = swapChainDetails.surfaceFormat.format;
		VkPresentModeKHR presentMode = chooseSwapPresentMode();
		swapChainDetails.extent = chooseSwapExtent(window);

		//defining how many images to use predefined by swapchain
		imageCount = swapChainDetails.capabilities.minImageCount + 1;
		if (imageCount > swapChainDetails.capabilities.maxImageCount)
			imageCount = swapChainDetails.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = window->GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = swapChainDetails.surfaceFormat.format;
		createInfo.imageColorSpace = swapChainDetails.surfaceFormat.colorSpace;
		createInfo.imageExtent = swapChainDetails.extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		//deciding which kind of image sharing mode is going to be used
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		if (indices.graphicsFamily != indices.presentFamily)
		{
			//not preffered
			DebugPrint("Using concurrent queue-family sharing mode, while exclusive is preffered.\n");
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{	//preffered
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		if (vkCreateSwapchainKHR(LD, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swap chain!");
	}

	//TODO: detect if we need to recreate the renderPass object in case swap chain image format changed
	void vSwapChain::RecreateSwapChain(std::shared_ptr<const Window> window, const QueueFamilyIndices& indices)
	{
		vkDeviceWaitIdle(LD);
		this->~vSwapChain();

		CreateSwapChain(window, indices);
		CreateImageViews();
		//return imageCount;
	}

	VkSurfaceFormatKHR vSwapChain::chooseSwapSurfaceFormat()
	{
		for (const auto& availableFormat : swapChainDetails.formats) 
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		//print if the preffered swapsurfaceformat is not discovered
		DebugPrint("picked first SwapSurfaceFormat available..\n");
		return swapChainDetails.formats[0];
	}

	VkPresentModeKHR vSwapChain::chooseSwapPresentMode()
	{
		for (const auto& availablePresentMode : swapChainDetails.presentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;		//if mailbox present mode is not available pick fifo (which is guaranteed to be available)
	}

	void vSwapChain::CreateImageViews()
	{
		if (images.size()) images.clear();
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(LD, swapChain, &imageCount, images.data());

		//creating imageviews
		for (uint32_t i = 0; i < imageCount; ++i) 
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			//	createInfo.format = VkFormat();			//learned that functions do not always return an error if validation would fail
			createInfo.format = swapChainDetails.swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkImageView imageView = nullptr;

			if (vkCreateImageView(LD, &createInfo, nullptr, &imageView) != VK_SUCCESS)
				throw std::runtime_error("failed to create image views!");

			imageViews.push_back(imageView);
		}
	}

	VkExtent2D vSwapChain::chooseSwapExtent(std::shared_ptr<const Window> window) {
		if (swapChainDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() &&
			swapChainDetails.capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
			return swapChainDetails.capabilities.currentExtent;
		else
		{
			vec2<int> screen;
			glfwGetFramebufferSize(const_cast<GLFWwindow*>(window->GetWindow()), &screen.x, &screen.y);
			DebugPrint("Querried GLFW for framebuffer sizes..\n");

			VkExtent2D actualExtent = {
				std::clamp(static_cast<uint32_t>(screen.x), swapChainDetails.capabilities.minImageExtent.width , swapChainDetails.capabilities.maxImageExtent.width),
				std::clamp(static_cast<uint32_t>(screen.y), swapChainDetails.capabilities.minImageExtent.height, swapChainDetails.capabilities.maxImageExtent.height)
			};

			return actualExtent;
		}
	}
	static int Fcount = 0;
	void vSwapChain::CreateFramebuffers(const VkRenderPass renderPass)
	{
		DebugPrint(std::string("Framebuffer init ") + std::to_string(++Fcount), '\n');

		framebuffers.resize(imageCount);
		VkFramebufferCreateInfo framebufferInfo{};
		for (size_t i = 0; i < imageCount; ++i)
		{
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = GetImageViews() + i;
			framebufferInfo.width = swapChainDetails.extent.width;
			framebufferInfo.height = swapChainDetails.extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(LD, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create framebuffer.");
			}
		}
	}
}