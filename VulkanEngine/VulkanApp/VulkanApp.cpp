#include "pch.h"
#include "VulkanApp.h"
#include "EngineApi/Core.h"

namespace VWrapper
{
	bool App::initialized = false;
	bool App::freeMemory = false;

	void App::Init()
	{
		assert(!initialized);	//If the app is already running... don't initalize it again	
		initialized = true;
		VulkanCore::Init(std::make_shared<Window>("V-Engine v.0"));

		objectArray.push_back(std::make_unique<BaseObject>());
	}

	//potentially add render draw loop
	//or define other physics funtions for loop.cpp

	std::vector<std::unique_ptr<BaseObject>> App::objectArray;

	void App::Shutdown()
	{
		assert(initialized);	//Initialize the app before requesting a shutdown
		VulkanCore::Clean();
	}

	//set up deltaTime
	void App::EntityUpdate()
	{

	}

	void App::PhysicsLoop()
	{

	}

	void App::Animate()
	{

	}

}