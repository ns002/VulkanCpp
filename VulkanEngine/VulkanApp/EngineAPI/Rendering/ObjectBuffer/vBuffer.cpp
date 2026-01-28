#include "pch.h"
#include "../../Device/DeviceHandler.h"
#include "../../Device/LogicalDevice.h"
#include "vBuffer.h"
#include "vBuffer.inl"

namespace VWrapper
{
	uint32_t vBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties; vkGetPhysicalDeviceMemoryProperties(vDeviceHandler::GetPhysicalDevicePtr(), &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		throw std::runtime_error("failed to find suitable memory type!");
	}

	vBuffer::vBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
		LD(vDeviceHandler::GetLogicalDevicePtr()),
		m_size(size), m_usage(usage), m_properties(properties)
	{
		CreateBuffer(m_size, m_usage, m_properties, m_buffer, m_bufferMemory);
	}

	vBuffer::~vBuffer()
	{
		vkDestroyBuffer(LD, m_buffer, nullptr);
		vkFreeMemory(LD, m_bufferMemory, nullptr);
	}

	void vBuffer::CreateBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		const VkDevice& LD = vDeviceHandler::GetLogicalDevicePtr();

		//creating buffer object
		VkMemoryRequirements memRequirements;
		VkBufferCreateInfo bufferInfo{};
		VkMemoryAllocateInfo allocInfo{};

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(LD, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer!");

		vkGetBufferMemoryRequirements(LD, buffer, &memRequirements);
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(LD, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		vkBindBufferMemory(LD, buffer, bufferMemory, 0);
	}

	void vBuffer::CopyBuffer(const vBuffer& srcBuffer, vBuffer& dstBuffer, const VkCommandPool& commandPool)
	{
		//comment asserts if you expect it to work, and want to see if it does... 
		//checks if buffers are correctly being used, and not being repopulated or something like that
		assert(srcBuffer.populated && srcBuffer.m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		assert(!dstBuffer.populated && dstBuffer.m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT);

		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocInfo{};
		VkCommandBufferBeginInfo beginInfo{};
		VkBufferCopy copyRegion{};
		VkSubmitInfo submitInfo{};

		const VkQueue& queue = vDeviceHandler::GetLogicalDeviceClass().GetGraphicsQueue();
		const VkDevice& LD = vDeviceHandler::GetLogicalDevicePtr();

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(LD, &allocInfo, &commandBuffer);

		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = dstBuffer.m_size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer.m_buffer, dstBuffer.m_buffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(LD, commandPool, 1, &commandBuffer);
		dstBuffer.populated = true;
	}

	//Populate a buffer with Vertex data
	template <typename T>
	SFINAE_POPULATE_VERTEX_T vBuffer::Populate(const std::vector<T>& srcData)
	{
		assert(!populated && (
			  (m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
			  (m_usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT && !(m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT))));
		
		void* destination;
		vkMapMemory(LD, m_bufferMemory, 0, m_size, 0, &destination);
		memcpy(destination, vertices.data(), m_size);	//might require vertex input at some point here
		vkUnmapMemory(LD, m_bufferMemory);
		populated = true;
	}

	//Populate a buffer with index data
	template <typename T>
	SFINAE_POPULATE_INTEGRAL_T vBuffer::Populate(const std::vector<T>& srcData)
	{
		assert(!populated && (
			  (m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
			  (m_usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT && !(m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT))));

		void* destination;
		vkMapMemory(LD, m_bufferMemory, 0, m_size, 0, &destination);
		memcpy(destination, indices.data(), m_size);	//might require vertex input at some point here
		vkUnmapMemory(LD, m_bufferMemory);
		populated = true;
	}

	//Catch-all template for unsupported types
	template <typename T>
	SFINAE_POPULATE_UNSUPPORTED_T vBuffer::Populate(const std::vector<T>& srcData)
	{
		throw std::runtime_error("vBuffer::PopulateBufferObject(B, vector<THIS>); Unsupported type: \"" + typeid(T).name() + "\".\nShould be of type 'Vertex' or Integral.");
	}
	
}

//let's not use any of these down the road, alright
#pragma warning(push)
#pragma warning(disable : 4005)
#define VBUFFER_H false
#undef VERTEX_TYPE
#undef INTEGRAL_TYPE
#undef SFINAE_POPULATE_VERTEX_T
#undef SFINAE_POPULATE_INTEGRAL_T
#undef SFINAE_POPULATE_UNSUPPORTED_T
#pragma warning(pop)