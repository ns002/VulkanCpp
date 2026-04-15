#pragma once
#include "pch.h"
#include "../Device/SupportStructure.h"

class Window;

namespace VWrapper
{
	class vLogicalDevice;
	class vSwapChain
	{
		friend class RenderPipeline;	//the pipeline IS the swapchain, or smth. lol

	public:
		vSwapChain(const VkDevice& LD, std::shared_ptr<Window> window, const QueueFamilyIndices& indices, std::optional<SwapChainSupportDetails>& swapChainSupport);
		~vSwapChain();

		inline const size_t size() { return imageCount; }
		inline const SwapChainSupportDetails& GetSwapChainDetails() { return swapChainDetails; }
		inline const VkImageView* const GetImageViews() { return imageViews.data(); }

	private:	//private only for use in renderpipeline class or here

		VkSwapchainKHR swapChain = nullptr;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;
		uint32_t imageCount;

		const VkDevice& LD;							//local ref of a ptr for quick access, should never get invalidated
		SwapChainSupportDetails& swapChainDetails;	//todo: can't this class just take ownership??...it should

		void CreateSwapChain(std::shared_ptr<const Window> window, const QueueFamilyIndices& indices);
		void RecreateSwapChain(std::shared_ptr<const Window> window, const QueueFamilyIndices& indices); //in case something fucks us up, so we won't crash and burn

		VkSurfaceFormatKHR chooseSwapSurfaceFormat();
		VkPresentModeKHR chooseSwapPresentMode();
		void CreateImageViews();
		VkExtent2D chooseSwapExtent(std::shared_ptr<const Window> window);
		void CreateFramebuffers(const VkRenderPass renderPass);

	};
}