#include "pch.h"
#include "VShaderCompiler.h"
#include "CompileSettings.inl"	//definitely not optimized but it works so dont touch it

#pragma warning(push)
// wow...thanks boost
#pragma warning(disable : 26110)
#pragma warning(disable : 26437)
#pragma warning(disable : 26439)
#pragma warning(disable : 26495)
#pragma warning(disable : 28251)
#pragma warning(disable : 4005)
#pragma warning(disable : 4244)
#pragma warning(disable : 6001)
#pragma warning(disable : 6031)
#pragma warning(disable : 6258)
#pragma warning(disable : 6387)
#pragma warning(disable : 6388)
#include <boost/process.hpp>

#if defined(_WIN32) || defined(_WIN64)	//windows dinosaur must flicker random command windows without content
#include <boost/process/windows.hpp>
#pragma warning(pop)
#define CMD_LAUNCH(childProc, command_cstr, pStream) childProc = std::make_unique<bp::child>(command_cstr, bp::std_err > pStream, bp::windows::create_no_window)
#else
#pragma warning(pop)
//#define CMD_LAUNCH(childProc, command_cstr, pStream) childProc = std::make_unique<bp::child>(command_cstr, bp::std_err > pStream)
#endif

namespace bp = boost::process;

namespace VWrapper
{
	void ShaderCompiler::CompileShader(const ShaderType&& command) noexcept(false)
	{
		std::string cmd = GetCommand(command); const char* dbgMsg[3] = {"ShaderCompiler: executing command... \"", cmd.c_str(), "\"\n\n"};
		DebugPrint(dbgMsg, 3, std::nullopt);
		int result = ExecuteCommand(cmd.c_str());
		if (static_cast<bool>(result))	//if non zero return code
		{
			std::string err = "Shadercompiler: {";
			err = err + ShaderTypeToStr(command) + "} command execution returned exit code: " + std::to_string(result);
			throw std::runtime_error(err.c_str());
		}
	}

	//was attempted to throw if unsuccesful but it remains largely untested
	void ShaderCompiler::Compile() noexcept(false)
	{
		FileSystemCheck();
		printf("\n[Starting shadercode compilation in 'glslc.exe']\n");

		//If compilation fails we currently throw an exception. (final call in ExecuteCommand())
		//We could also check if there were previous versions and ask to run with those.. no need to necessarily throw
		CompileShader(ShaderType::VERTEX);
		CompileShader(ShaderType::FRAGMENT);
		
		printf("[Finalised]\n\n");	//this is only printed when succesfull
	}

	//attempting to create a folder to place spv files if this does not exist yet
	void ShaderCompiler::FileSystemCheck()
	{
		if (!exists(COMPILED_SHADER_LOCATION))
		{
			try { create_directory(COMPILED_SHADER_LOCATION); }
			catch (const std::exception& e)		//it is possible that you fail to create a folder
			{
				throw std::runtime_error(std::string("Failed to create \"") + SHADER_DESTINATION_FOLDER + "\" folder. Exception:\n" + e.what() + '\n');			//this assumes that you wan't to use a shader.. if you cannot output one, there is no point to continue
			}
		}
	}

	const std::string ShaderCompiler::GetCommand(const ShaderType& command)
	{
		switch (command)
		{
		case ShaderType::VERTEX:	return VERTEX_COMMANDLINE  (ShaderCompiler::FindGls(glslc));
		case ShaderType::FRAGMENT:	return FRAGMENT_COMMANDLINE(ShaderCompiler::FindGls(glslc));
		//why would i want a default path?
		default: return "";		//untested
		}
	}

	int ShaderCompiler::ExecuteCommand(const char* command) noexcept(false)
	{
		try {
			bp::ipstream errorStream;
			std::unique_ptr<bp::child> childProces = nullptr;
			CMD_LAUNCH(childProces, command, errorStream);	//launches the shader compiler process (will wait until cmd completion)

			std::string log;
			while (errorStream && std::getline(errorStream, log) && !log.empty())	//read childs cerr buffer line by line
			{
				//the file path is before the error text and we don't need that. We already know the files we mean to debug
				//we are going to try to remove that part of the log-line (IF it is there)
				size_t cutoff = log.find(".vert");
				if (cutoff == MAXSIZE_T) cutoff = log.find(".frag");
				if (cutoff == MAXSIZE_T) DebugPrint(log.append(1, '\n').c_str());				//just printing & adding a '\n'
				else DebugPrint(("Line " + log.substr(cutoff + 6).append(1, '\n')).c_str());	//there is file specific debug info
			}

			childProces->wait();
			return childProces->exit_code();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error((std::string("Failed to execute shadercommand. Details:\n ") + e.what() + "\n  Also Check if the path above is correct.  You can change them in 'CompileSettings.inl'").c_str());
		}
	}

	std::optional<std::filesystem::path> ShaderCompiler::FindGls(const char* exe) {

		// 1. Try VULKAN_SDK
		if (const char* sdk = std::getenv("VULKAN_SDK")) {
			std::filesystem::path p = std::filesystem::path(sdk) / "Bin" / exe;
			if (std::filesystem::exists(p))
				return std::filesystem::canonical(p);
		}

		// 2. Fallback: search PATH
		if (const char* pathEnv = std::getenv("PATH")) {
			std::stringstream ss(pathEnv);
			std::string dir;

			while (std::getline(ss, dir, path_sep)) {
				std::filesystem::path p = std::filesystem::path(dir) / exe;
				if (std::filesystem::exists(p))
					return std::filesystem::canonical(p);
			}
		}

		return std::nullopt;
	}
}