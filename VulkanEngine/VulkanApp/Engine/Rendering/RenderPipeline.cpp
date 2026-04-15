#include "pch.h"
#include "../Core.h"
#include "../Device/DeviceHandler.h"
#include "../Device/LogicalDevice.h"
#include "../Device/PhysicalDevice.h"
#include "RenderPipeline.h"
#include "../Shader/ShaderHandler.h"
#include "Mesh.h"
#include "SwapChain.h"

namespace VWrapper
{

	RenderPipeline::RenderPipeline(vSwapChain* ptrSwapChain) :	
		LD(vDeviceHandler::GetLogicalDevicePtr()),
		swapChain(ptrSwapChain)	//pointer will be deleted in .clean()
	{
		//lots of setupping to do before we get a functional pipeline
		CreateRenderPass();
		if (swapChain == nullptr) throw std::runtime_error("RenderPipeline Init: passed swapchain ptr is null.");
		swapChain->CreateFramebuffers(renderPass);
		CreateRenderPipeline(swapChain->GetSwapChainDetails().extent);
		CreateCommandPool();
		mesh = new Mesh(commandPool, swapChain->GetSwapChainDetails().capabilities.maxImageCount, descriptorSetLayout);
		CreateCommandBuffer();
		CreateSyncObjects();
	}

	void RenderPipeline::Clean()
	{
		WaitForFences();
		DestroySynchObjects();
		vkDestroyCommandPool(LD, commandPool, nullptr);
		vkDestroyPipeline(LD, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(LD, pipelineLayout, nullptr);
		vkDestroyRenderPass(LD, renderPass, nullptr);
		if (swapChain != nullptr) delete swapChain;
		if (mesh != nullptr) mesh->DestroyUniformBuffers();
		vkDestroyDescriptorSetLayout(LD, descriptorSetLayout, nullptr);
		if (mesh != nullptr) delete mesh;
	}

	bool RenderPipeline::DrawProc(const size_t& frameNum) const
	{
		uint32_t imageIndex;
		const VkSemaphore signalSemaphores[] = { renderFinishedSemaphore },
			waitSemaphores[] = { imageAvailableSemaphore };

		VkResult NextImage = vkAcquireNextImageKHR(LD, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		switch (NextImage)
		{
		default: throw std::runtime_error("Failed to aquire swap chain image!");	//if this fails expand debugging operation

		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			DebugPrint("Recreating SwapChain...\n");
			swapChain->swapChainDetails = vDeviceHandler::GetPhysicalDeviceClass().querySwapChainSupport();
			swapChain->RecreateSwapChain(VulkanCore::GetWindow(), vDeviceHandler::GetQueueFamilyIndices());
			swapChain->CreateFramebuffers(renderPass);
			DestroySynchObjects();
			CreateSyncObjects();	//Synch objects will be invalidated so recreate
			return this->DrawProc(frameNum);

		case VK_SUCCESS:
			WaitForFences();
			ResetFences();
			vkResetCommandBuffer(commandBuffer, 0);
			RecordCommandBuffer(imageIndex);
			mesh->UpdateUniformBuffer(imageIndex, swapChain->swapChainDetails.extent);
			SubmitCommandBuffer(signalSemaphores, waitSemaphores);
			Present(signalSemaphores, waitSemaphores, imageIndex);
			break;
		}

		if (frameNum == 1) return true; else return false;			//dirty way to show the window on completion of the first frame instead of after initialization
	}

	void RenderPipeline::WaitForFences()	   const { vkWaitForFences(LD, 1, &inFlightFence, VK_TRUE, UINT64_MAX); }
	void RenderPipeline::ResetFences()		   const { vkResetFences(LD, 1, &inFlightFence); }
	void RenderPipeline::DestroySynchObjects() const
	{
		vkDestroySemaphore(LD, renderFinishedSemaphore, nullptr);
		vkDestroyFence(LD, inFlightFence, nullptr);
		vkDestroySemaphore(LD, imageAvailableSemaphore, nullptr);
	}

	//i think we did this somewhere else just incomplete...dig around to find it?
	//this is the correct way to do it though!
	void RenderPipeline::RecordCommandBuffer(const uint32_t& imageIndex) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		VkRenderPassBeginInfo renderPassInfo{};
		const VkExtent2D& swapchainExtent = swapChain->GetSwapChainDetails().extent;

		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChain->framebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainExtent;
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };	//any arbitrary background color
		//black = { {{0.0f, 0.0f, 0.0f, 1.0f}} }
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		VkBuffer vertexBuffers[] = { mesh->GetVbo().buffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, mesh->GetIbo().buffer(), 0, VK_INDEX_TYPE_UINT16);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchainExtent.width;
		viewport.height = (float)swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &mesh->GetDescriptorSet(imageIndex), 0, nullptr);

		//uint32_t vertexCount = static_cast<uint32_t>(mesh->GetVbo().size()),
		//         instanceCount = 1;
		//vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);							//for VertexBuffer drawing
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);	//for drawing VertexBuffer with IndexBuffer included

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}

	void RenderPipeline::SubmitCommandBuffer(const VkSemaphore* signalSemaphores, const VkSemaphore* waitSemaphores) const
	{
		VkSubmitInfo submitInfo{};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(vDeviceHandler::GetLogicalDeviceClass().GetGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
	}
	
	void RenderPipeline::Present(const VkSemaphore* signalSemaphores, const VkSemaphore* waitSemaphores, const uint32_t& imageIndex) const
	{
		VkSwapchainKHR swapChains[] = { swapChain->swapChain };
		VkPresentInfoKHR presentInfo{};

		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(vDeviceHandler::GetLogicalDeviceClass().GetPresentQueue(), &presentInfo);
	}

	void RenderPipeline::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		VkAttachmentReference colorAttachmentRef{};
		VkSubpassDescription subpass{};
		VkSubpassDependency dependency{};
		VkRenderPassCreateInfo renderPassInfo{};

		//In our case we'll have just a single color buffer attachment represented by one of the images from the swap chain.
		colorAttachment.format = swapChain->GetSwapChainDetails().swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//
		//The following other types of attachments can be referenced by a subpass:
		//
		//pInputAttachments:		Attachments that are read from a shader
		//pResolveAttachments:		Attachments used for multisampling color attachments
		//pDepthStencilAttachment:	Attachment for depth and stencil data
		//pPreserveAttachments:		Attachments that are not used by this subpass, but for which the data must be preserved

		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(LD, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create render pass.");
		}
	}

	//there are a lot of options here
	void RenderPipeline::CreateRenderPipeline(const VkExtent2D& extent)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};

		VkVertexInputBindingDescription bindingDescription{};
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportState{};

		VkPipelineRasterizationStateCreateInfo rasterizer{};

		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};

		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		VkGraphicsPipelineCreateInfo pipelineInfo{};

		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(LD, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		//The pVertexBindingDescriptions and pVertexAttributeDescriptions members point to an array of structs that describe the "aforementioned" details for loading vertex data.
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		

		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		//dynamic
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		scissor.offset = { 0,0 };
		scissor.extent = extent;

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//VK_POLYGON_MODE_FILL		will: fill rectangle
		//K_POLYGON_MODE_LINE		will: wireframe
		//VK_POLYGON_MODE_POINT		will: vertices are drawn as points
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f; //any line thicker than 1.0f requires you to enable the wideLines GPU feature.
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		//specifies the vertex order for faces to be considered front-facing and can be clockwise or counterclockwise.
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		//this can be used for shadow mapping (all optional)
		rasterizer.depthBiasEnable = VK_FALSE; //not optional
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;				// Optional
		multisampling.pSampleMask = VK_NULL_HANDLE;				// Optional
		multisampling.alphaToCoverageEnable = VK_FALSE;		// Optional
		multisampling.alphaToOneEnable = VK_FALSE;			// Optional

		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;	// Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;				// Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;		// Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;	// Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;				// Optional

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;	//enable if blendEnable (above) is also set to TRUE
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE; // Optional

		if (vkCreatePipelineLayout(LD, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout.");

		//Creating the actual pipeline and defining the create info for it
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pStages = ShaderHandler::GetShaderStageCreateInfo();
		pipelineInfo.stageCount = shaderStagesArrSize;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = VK_NULL_HANDLE; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(LD, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline.");
	}

	void RenderPipeline::CreateCommandPool()
	{
		const QueueFamilyIndices& queueFamilyIndices = vDeviceHandler::GetQueueFamilyIndices();
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(LD, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	/* TODO:
	Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls.
	You have to record all of the operations you want to perform in command buffer objects.
	The advantage of this is that when we are ready to tell the Vulkan what we want to do, all of the commands are submitted together and
	Vulkan can more efficiently process the commands since all of them are available together. In addition,
	this allows command recording to happen in multiple threads if so desired.
	*/
	void RenderPipeline::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(LD, &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void RenderPipeline::CreateSyncObjects() const
	{
		VkSemaphoreCreateInfo semaphoreInfo{}; VkFenceCreateInfo fenceInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(LD, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(LD, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(LD, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}

}