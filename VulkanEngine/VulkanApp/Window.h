#pragma once
#include "pch.h"

//TODO: create base window class and make this class a child of that
// Called Editor window or something like that

//TODO: make a fallback system for when vulkan does not initialize...
// cuz vulkan does break sometimes

//Will hold all window & monitor related functions.
//And other related functionalities
class Window
{

public:
	Window(std::string name);
	
	//remember to override deconstructor (don't want to terminate glfw with "parent window" still running.
	virtual ~Window();
	
	void DestroyWindow(const VkInstance& instance);
	void CreateVkSurface(const VkInstance& instance);

	void Hide() const { glfwHideWindow(window); DebugPrint("Window hidden.\n"); }
	void Show() const { glfwShowWindow(window); DebugPrint("Window shown.\n"); }
	inline const bool ShouldClose() const { return glfwWindowShouldClose(window); }
	inline const bool IsClosed() const { return isOpen; }

	inline const VkSurfaceKHR& GetSurface() const { return surface; }
	inline const GLFWwindow* GetWindow() const { return window; }

private:

	void InitGlfwWindow();

	bool isOpen = false;
	std::string windowName;

	GLFWwindow* window = nullptr;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
};