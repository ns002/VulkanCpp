#include "pch.h"
#include "ShaderHandler.h"
#include "Shader.h"

namespace VWrapper
{
	std::map<const ShaderType, std::shared_ptr<vShader>> ShaderHandler::shaderMap;
	
	VkPipelineShaderStageCreateInfo ShaderHandler::shaderStages[2] = { {}, {} };

	bool ShaderHandler::AppendShader(const ShaderType&& type, const char* filename)
	{
		return shaderMap.try_emplace(type, std::make_shared<vShader>(type, filename)).second;
	}

	bool ShaderHandler::AppendShader(const ShaderType&& type)
	{
		const char* compiledFilename;

		switch (type)
		{
		case ShaderType::VERTEX:
			compiledFilename = OUTPUT_VERTEX_SHADER_NAME;
			break;
		case ShaderType::FRAGMENT:
			compiledFilename = OUTPUT_FRAGMENT_SHADER_NAME;
			break; 
		
		case ShaderType::Default:
		default: throw std::runtime_error(std::string("ShaderHandler::AppendShader(): You didn't specify which output filename, to use as default, for ") + ShaderTypeToStr(type) + " shader.\n code: " + std::to_string(int(type)));
		}

		return shaderMap.try_emplace(type, std::make_shared<vShader>(type, compiledFilename)).second;
	}

	const vShader* ShaderHandler::GetShader(const ShaderType shaderToGet)
	{
		try { return shaderMap.at(shaderToGet).get(); }
		catch (const std::exception& e) {
			const char* temp[3] = { (const char*)(e.what()), "GetShaderModule() invalid shader:", ShaderTypeToStr(shaderToGet)}; DebugPrint(temp, 3, " ", '\n');
			return nullptr;	//could throw again but lets carry on since this's recoverable
		}
	}


	size_t ShaderHandler::DestroyShader(const ShaderType shader)
	{
		return shaderMap.erase(shader);
	}

	void ShaderHandler::Clean()
	{
		shaderMap.clear();
	}

	void ShaderHandler::ShaderStageInfo()
	{
		auto vertex = GetShader(ShaderType::VERTEX);
		auto fragment = GetShader(ShaderType::FRAGMENT);
		if (!(vertex && fragment)) throw std::runtime_error("Could not find shader modules, aborted startup.");

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};	//this is redundant but makes code more clear
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertex->GetShader();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};	//this is redundant but makes code more clear
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragment->GetShader();
		fragShaderStageInfo.pName = "main";

		shaderStages[0] = vertShaderStageInfo; shaderStages[1] = fragShaderStageInfo;
	}
}