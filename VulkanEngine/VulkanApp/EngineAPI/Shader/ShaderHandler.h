#pragma once
#include "pch.h"
#include "CompileSettings.inl"
#include "ShaderType.h"


namespace VWrapper
{
	constexpr size_t shaderStagesArrSize = 2;

	class vShader;
	class ShaderHandler
	{
	public:

		static bool AppendShader(const ShaderType&& type, const char* filename);
		static bool AppendShader(const ShaderType&& type);

		static const vShader* GetShader(const ShaderType shaderToGet);
		
		//returning 0 would mean it didnt delete anything
		static size_t DestroyShader(const ShaderType shader);
		static void Clean();

		static void ShaderStageInfo();
		static const VkPipelineShaderStageCreateInfo* const GetShaderStageCreateInfo() { return shaderStages; }

	private:
		ShaderHandler() = delete;
		ShaderHandler(ShaderHandler&) = delete;

		static std::map<const ShaderType, std::shared_ptr<vShader>> shaderMap;
		static VkPipelineShaderStageCreateInfo shaderStages[shaderStagesArrSize];
	};
}