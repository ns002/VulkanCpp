#pragma once
#include "pch.h"
#include "ObjectBuffer\vBuffer.h"

namespace VWrapper
{
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Mesh
	{
	public:
		Mesh(const VkCommandPool& commandPool, const uint32_t& maxFramesInFlight, const VkDescriptorSetLayout& descriptorSetLayout);
		~Mesh();

		void CreateUniformBuffers(const uint32_t& maxFramesInFlight);
		void UpdateUniformBuffer(const uint32_t& imageIndex, const VkExtent2D& imageExtent);	//temp
		void DestroyUniformBuffers();

		void CreateDescriptorPool();
		void CreateDescriptorSets(const VkDescriptorSetLayout& descriptorSetLayout);

		const VkDescriptorSet& GetDescriptorSet(const size_t& index) { return descriptorSets[index]; }
		const vBuffer& GetVbo() { return *vbo; }
		const vBuffer& GetIbo() { return *ibo; }


	private:

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		vBuffer* vbo = nullptr;
		vBuffer* ibo = nullptr;

		uint32_t maxFrames;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		void CreateUBO(const VkDeviceSize& bufferSize, const size_t& index);
	};
}