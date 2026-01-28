#include "pch.h"
#include <sstream>  
#include <fstream>

#include "../Device/DeviceHandler.h"
#include "CompileSettings.inl"
#include "Shader.h"


namespace VWrapper
{
	vShader::vShader(const ShaderType a_type, const char* fileName) :
		type(a_type)
	{
		shader = CreateShaderModule(COMPILED_SHADER_LOCATION.append(fileName));
	}

	vShader::~vShader()
	{
		vkDestroyShaderModule(vDeviceHandler::GetLogicalDevicePtr(), shader, nullptr);
	}

	//very clean nice!
	const char* vShader::ReadShaderFile(std::filesystem::path& path, size_t* size)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) throw std::runtime_error("Failed to open shader file.");

		if (size == nullptr || (*size = static_cast<int>(file.tellg())) <= 0) 
			throw std::runtime_error("Shader loading: invalid filesize.");

		char* buffer = new char[*size];
		file.seekg(0); file.read(buffer, *size);
		file.close(); return buffer;
	}

	VkShaderModule vShader::CreateShaderModule(std::filesystem::path& path)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(ReadShaderFile(path, &createInfo.codeSize));

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vDeviceHandler::GetLogicalDevicePtr(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			std::stringstream debug;
			debug << "Failed to create shader module '" << ShaderTypeToStr(type) << '\'';
			delete createInfo.pCode;

			throw std::runtime_error(debug.str());
		}

		delete createInfo.pCode;
		return shaderModule;
	}
}