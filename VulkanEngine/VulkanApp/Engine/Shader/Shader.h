#pragma once
#include "pch.h"
#include <filesystem>

#include "ShaderType.h"

namespace VWrapper
{
	class vShader
	{
	public:

		const ShaderType type;

		vShader(const ShaderType a_type, const char* fileName);
		~vShader();
		
		const VkShaderModule& GetShader() const { return shader; }

	private:

		VkShaderModule shader = nullptr;

		static const char* ReadShaderFile(std::filesystem::path& path, size_t* size);
		VkShaderModule CreateShaderModule(std::filesystem::path& path);
	};
}