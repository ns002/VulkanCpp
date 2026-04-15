#include "pch.h"
#include "Mesh.h"
#include "../Device/DeviceHandler.h"

namespace VWrapper
{
	Mesh::Mesh(const VkCommandPool& commandPool, const uint32_t& maxFramesInFlight, const VkDescriptorSetLayout& descriptorSetLayout)
	{
		VkDeviceSize vboSize = sizeof(Vertex) * vertices.size();
		VkDeviceSize iboSize = sizeof(uint16_t) * indices.size();
		{
			vBuffer stagingBuffer{
				vboSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.Populate(vertices);

			vbo = new vBuffer(
				vboSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			vBuffer::CopyBuffer(stagingBuffer, *vbo, commandPool);
		}
		{
			vBuffer stagingBuffer {
				iboSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.Populate(indices);
		
			ibo = new vBuffer(
				iboSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			vBuffer::CopyBuffer(stagingBuffer, *ibo, commandPool);
		}

		CreateUniformBuffers(maxFramesInFlight);
		CreateDescriptorPool();
		CreateDescriptorSets(descriptorSetLayout);

		VkDescriptorBufferInfo bufferInfo{};
		VkWriteDescriptorSet descriptorWrite{};

		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE; //sizeof(UniformBufferObject)

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		for (size_t i = 0; i < maxFrames; i++) {
			bufferInfo.buffer = uniformBuffers[i];
			descriptorWrite.dstSet = descriptorSets[i];
			vkUpdateDescriptorSets(vDeviceHandler::GetLogicalDevicePtr(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	Mesh::~Mesh()
	{
		vkDestroyDescriptorPool(vDeviceHandler::GetLogicalDevicePtr(), descriptorPool, nullptr);
		if (uniformBuffers.size()) DestroyUniformBuffers();
		if (vbo != nullptr) delete vbo;
		if (ibo != nullptr) delete ibo;
	}
	
	

	void Mesh::CreateUniformBuffers(const uint32_t& maxFramesInFlight)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(maxFramesInFlight);
		uniformBuffersMemory.resize(maxFramesInFlight);
		uniformBuffersMapped.resize(maxFramesInFlight);

		for (size_t i = 0; i < maxFramesInFlight; i++) {
			CreateUBO(bufferSize, i);
		}

		maxFrames = maxFramesInFlight;
	}

	//static float time = 0.0f;
	static UniformBufferObject ubo{};
	void Mesh::UpdateUniformBuffer(const uint32_t& imageIndex, const VkExtent2D& imageExtent)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//time += 0.00694444444f; //just an indication of how damn fast this is/was (1/144 = 0.00694444444) it spins around more than once per second

		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), imageExtent.width / (float)imageExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[imageIndex], &ubo, sizeof(ubo));
	}

	void Mesh::DestroyUniformBuffers()
	{
		for (size_t i = 0; i < maxFrames; i++) {
			vkDestroyBuffer(vDeviceHandler::GetLogicalDevicePtr(), uniformBuffers[i], nullptr);
			vkFreeMemory(vDeviceHandler::GetLogicalDevicePtr(), uniformBuffersMemory[i], nullptr);
		}

		uniformBuffers.clear();
		uniformBuffersMemory.clear();
		uniformBuffersMapped.clear();
	}

	void Mesh::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		VkDescriptorPoolCreateInfo poolInfo{};

		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = maxFrames;

		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = maxFrames;

		if (vkCreateDescriptorPool(vDeviceHandler::GetLogicalDevicePtr(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");
	}
	
	void Mesh::CreateDescriptorSets(const VkDescriptorSetLayout& descriptorSetLayout)
	{
		std::vector<VkDescriptorSetLayout> layouts(maxFrames, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = maxFrames;
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(maxFrames);
		if (vkAllocateDescriptorSets(vDeviceHandler::GetLogicalDevicePtr(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate descriptor sets!");
	}

	void Mesh::CreateUBO(const VkDeviceSize& bufferSize, const size_t& index)
	{
		vBuffer::CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[index], uniformBuffersMemory[index]);

		vkMapMemory(vDeviceHandler::GetLogicalDevicePtr(), uniformBuffersMemory[index], 0, bufferSize, 0, &uniformBuffersMapped[index]);
	}

	
}