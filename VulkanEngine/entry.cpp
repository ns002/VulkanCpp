#include "pch.h"
#include "VulkanApp/VulkanApp.h"

using namespace VWrapper;

//TODO: set the application to WINDOWS/subsystem
//		write errors to debug log somewhere
//		make a debug log in general.. console sucks lol

//All the windows shit will make this file a lot cleaner

//for implicitly showing a console window: https://stackoverflow.com/questions/2139637/hide-console-of-windows-application/6882500#6882500
//#pragma comment(linker, "/SUBSYSTEM:console")
int main()
{
	DebugPrint("App startup.\n");
	int returnCode = EXIT_SUCCES;

	try { App::Init();
		try { App::Run();
			try { App::Shutdown(); }
			catch (const std::runtime_error& e) {	//catch init::run::shutdown
				DebugPrint(std::string("\nRuntime error occured when quitting. Given error information:\n") + e.what() + '\n');
				SleepS(sleepTime); returnCode = EXIT_FAILURE;
		}	}
		catch (const std::runtime_error& e) {		//catch init::run
			DebugPrint(std::string("\nRuntime error occured. Given error information:\n") + e.what() + '\n');
			try { App::Shutdown(); }
			catch (const std::runtime_error& e) {	//catch init::run::shutdown
				DebugPrint(std::string("\nAn Additional runtime error occured when quitting. Given error information:\n") + e.what() + '\n');
				SleepS(sleepTime); returnCode = EXIT_FAILURE;
			}
			SleepS(sleepTime); returnCode = EXIT_FAILURE;
	}	}
	catch (const std::runtime_error& e) {			//catch init::run
		DebugPrint(std::string("\nRuntime error occured during startup. Given error information:\n") + e.what() + '\n');
		try { App::Shutdown(); }
		catch (const std::runtime_error& e) {
			DebugPrint(std::string("\n An additional runtime error occured when quitting. Given error information:\n") + e.what() + '\n');
			SleepS(sleepTime); returnCode = EXIT_FAILURE;
		}
		SleepS(sleepTime); returnCode = EXIT_FAILURE;
	}
	returnCode = EXIT_SUCCES;
	DebugPrint(std::string("Return code is: ") + std::to_string(returnCode), '\n'); SleepS(sleepTime);
	return returnCode;
}
