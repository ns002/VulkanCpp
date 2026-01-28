#pragma once
#include "pch.h"

namespace VWrapper
{
	class Mesh;
	class vBuffer;
	class vSwapChain;
	class RenderPipeline
	{
	public:
		RenderPipeline(vSwapChain* ptrSwapChain);	//we take ownership of the passed pointer
		~RenderPipeline() = default;	//Clean() function is used instead
		void Clean();

		bool DrawProc(const size_t& frameNum) const;
		

	private:

		void WaitForFences() const;
		void ResetFences() const;
		void DestroySynchObjects() const;
		void RecordCommandBuffer(const uint32_t& imageIndex) const;
		void SubmitCommandBuffer(const VkSemaphore* signalSemaphores, const VkSemaphore* waitSemaphores) const;
		void Present(const VkSemaphore* signalSemaphores, const VkSemaphore* waitSemaphores, const uint32_t& imageIndex) const;

		void CreateRenderPass();
		void CreateRenderPipeline(const VkExtent2D& extent);
		void CreateCommandPool();
		
		void CreateCommandBuffer();
		void CreateSyncObjects() const;

		const VkDevice& LD;					//local ref of a ptr for quick access, should never get invalidated
		vSwapChain* swapChain = nullptr;	//we are passed a pointer -> we take ownership in this class [See RenderPipeline::Clear()]
		VkPipeline graphicsPipeline = nullptr;
		VkRenderPass renderPass = nullptr;
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;
		VkCommandPool commandPool = nullptr;
		Mesh* mesh = nullptr;
		VkCommandBuffer commandBuffer = nullptr;
		mutable VkSemaphore imageAvailableSemaphore = nullptr;
		mutable VkSemaphore renderFinishedSemaphore = nullptr;
		mutable VkFence inFlightFence = nullptr;
	};
}