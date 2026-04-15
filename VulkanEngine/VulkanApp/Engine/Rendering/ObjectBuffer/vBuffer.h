#pragma once
#include "pch.h"
#include "Vertex.h"

#include "vBuffer.inl"

namespace VWrapper
{
	class vBuffer
	{
	public:
		vBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);	//this will prevent making a copy accidentally
		~vBuffer(); vBuffer() = delete;

		const VkDeviceSize&		size()	 const {return m_size;}
		const VkBuffer&			buffer() const {return m_buffer;}
		const VkDeviceMemory&	memory() const {return m_bufferMemory;}

		//static helper function (exposes buffer creation logic)
		static void CreateBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		//optimized way of populating buffer objects (with non-accesible data in gpu)
		static void CopyBuffer(const vBuffer& srcBuffer, vBuffer& dstBuffer, const VkCommandPool& commandPool);
		
		//Populate buffer functions are down below
		//To keep their definitions and declarations together while outside of class

	private:
		const VkDevice& LD;					//local ref of a ptr for quick access, should never get invalidated (the amount of copies...)
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	protected:
		bool populated = false;

		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;
		VkDeviceSize m_size;
		VkBufferUsageFlags m_usage;			//Usage flags (e.g., vertex, transfer src/dst)
		VkMemoryPropertyFlags m_properties;	//Memory properties (e.g., host-visible, device-local)
	
	public:
		/*Template declarations as SFINAE(Substitution Failure Is Not An Error)*/
		//template verteces
		template <typename T>
		SFINAE_POPULATE_VERTEX_T	  Populate(const std::vector<T>& srcData);
		//template indeces
		template <typename T>
		SFINAE_POPULATE_INTEGRAL_T	  Populate(const std::vector<T>& srcData);
		//Catch-all template for unsupported types
		template <typename T>
		SFINAE_POPULATE_UNSUPPORTED_T Populate(const std::vector<T>& srcData);
	};

	//Explicit template instantiation declarations
	template void vBuffer::Populate<Vertex>  (const std::vector<Vertex>& srcData);
	template void vBuffer::Populate<uint16_t>(const std::vector<uint16_t>& srcData);
	template void vBuffer::Populate<uint32_t>(const std::vector<uint32_t>& srcData);
}

//let's not use any of these down the road, alright
//only in vBuffer.cpp
#pragma warning(push)
#pragma warning(disable : 4005)
#define VBUFFER_H false
#undef VERTEX_TYPE
#undef INTEGRAL_TYPE
#undef SFINAE_POPULATE_VERTEX_T
#undef SFINAE_POPULATE_INTEGRAL_T
#undef SFINAE_POPULATE_UNSUPPORTED_T
#pragma warning(pop)