#pragma once
#include "pch.h"
#include "ShaderType.h"
namespace VWrapper
{
	//compiling shaders on the fly by running command line operations in the background
	//as long as vulkan dev kit is installed

	class ShaderCompiler 
	{
		ShaderCompiler() = delete;	//deleted constructors as you shouldn't create an instance of any kind
		ShaderCompiler(const ShaderCompiler&) = delete;
		~ShaderCompiler() = delete;

	public:

		//compiles single shader file
		static void CompileShader(const ShaderType&& command) noexcept(false);

		//compile standard vertex and fragment shader
		//was attempted to throw if unsuccesful but it remains largely untested
		static void Compile() noexcept(false);

	private:

		static std::optional<std::filesystem::path> FindGls(const char* exe);
		static void FileSystemCheck() noexcept(false);							//attempting to create a folder to place spv files if this does not yet exist
		static const std::string GetCommand(const ShaderType& command);
		static int ExecuteCommand(const char* command) noexcept(false);
		
	};
} 