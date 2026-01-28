#pragma once
#include <filesystem>
//you wanted an engine yes?

//where you installed vulkan and the version
//this is the standard install location of Vulkan. installed in C: -> VulkanSDK -> version etc

//BE SURE TO APPLY YOUR CHANGES WITH A FULL REBUILD

#define SHADER_VALIDATOR_LOCATION	std::filesystem::path("E:\\VulkanSDK\\1.3.261.1\\Bin\\glslangValidator.exe")	//Maybe make computer check if it's C then E ..somehow
#define SHADER_COMPILER_LOCATION	std::filesystem::path("E:\\VulkanSDK\\1.3.261.1\\Bin\\glslc.exe")	//On my pc it's E:\\same

namespace VWrapper //inside namsepace should be global across any machine
{
	constexpr const char* SHADER_DESTINATION_FOLDER = "shader_dump";	//i'm getting tired of calling it temp

	constexpr const char* VERTEX_SHADER_NAME = "startup.vert";
	constexpr const char* FRAGMENT_SHADER_NAME = "startup.frag";

	constexpr const char* OUTPUT_VERTEX_SHADER_NAME = "startup.vert.spv";
	constexpr const char* OUTPUT_FRAGMENT_SHADER_NAME = "startup.frag.spv";
}

#define SHADER_LOCATION             std::filesystem::current_path().parent_path().parent_path().parent_path().append("VulkanEngine").append("Shaders")
#define COMPILED_SHADER_LOCATION	std::filesystem::current_path().parent_path().parent_path().append(SHADER_DESTINATION_FOLDER)


#define VERTEX_COMMANDLINE          ("\"" + SHADER_COMPILER_LOCATION.string() + "\" \"" + SHADER_LOCATION.append(VERTEX_SHADER_NAME).string() + "\" -o \"" + COMPILED_SHADER_LOCATION.append(OUTPUT_VERTEX_SHADER_NAME).string() + '\"')
#define FRAGMENT_COMMANDLINE        ("\"" + SHADER_COMPILER_LOCATION.string() + "\" \"" + SHADER_LOCATION.append(FRAGMENT_SHADER_NAME).string() + "\" -o \"" + COMPILED_SHADER_LOCATION.append(OUTPUT_FRAGMENT_SHADER_NAME).string() + '\"')