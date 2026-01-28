#pragma once
#include <filesystem>
namespace VWrapper //inside namsepace should be global across any machine
{
	constexpr const char* glslc = "glslc.exe";
	constexpr const char* glslangValidator = "glslangValidator.exe";
	constexpr const char path_sep = ';';
	constexpr const char* SHADER_DESTINATION_FOLDER = "shader_dump";	//i'm getting tired of calling it temp

	constexpr const char* VERTEX_SHADER_NAME = "startup.vert";
	constexpr const char* FRAGMENT_SHADER_NAME = "startup.frag";

	constexpr const char* OUTPUT_VERTEX_SHADER_NAME = "startup.vert.spv";
	constexpr const char* OUTPUT_FRAGMENT_SHADER_NAME = "startup.frag.spv";
}

#define SHADER_LOCATION             std::filesystem::current_path().parent_path().parent_path().parent_path().append("VulkanEngine").append("Shaders")
#define COMPILED_SHADER_LOCATION	(std::filesystem::current_path().parent_path() / SHADER_DESTINATION_FOLDER)


#define VERTEX_COMMANDLINE(CompilerLoc)          ("\"" + CompilerLoc.value().string() + "\" \"" + SHADER_LOCATION.append(VERTEX_SHADER_NAME).string() + "\" -o \"" + COMPILED_SHADER_LOCATION.append(OUTPUT_VERTEX_SHADER_NAME).string() + '\"')
#define FRAGMENT_COMMANDLINE(CompilerLoc)        ("\"" + CompilerLoc.value().string() + "\" \"" + SHADER_LOCATION.append(FRAGMENT_SHADER_NAME).string() + "\" -o \"" + COMPILED_SHADER_LOCATION.append(OUTPUT_FRAGMENT_SHADER_NAME).string() + '\"')