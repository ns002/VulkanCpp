#pragma once
#include "Window.h"
#include "Game/Object/BaseObject.h"

#undef EXIT_FAILURE
#undef EXIT_SUCCESS
#define EXIT_SUCCES			0
#define EXIT_FAILURE			100
#define EXIT_FAILED_TO_LAUNCH	101


namespace VWrapper
{
	class App
	{
	public:
		static void Init();
		static void Run();
		static void Shutdown();

	private: App() = delete; ~App() = delete;
		App(const App&) = delete;
		App& operator=(const App&) = delete;
	
	public:		
				
		static bool initialized;
		static bool freeMemory;

		static std::vector<std::unique_ptr<BaseObject>> objectArray;
		static void EntityUpdate();
		static void PhysicsLoop();
		static void Animate();
	};
}