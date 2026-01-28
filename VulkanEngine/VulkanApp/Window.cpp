#include "pch.h"
#include "Window.h"

Window::Window(std::string name) :
	windowName(name)
{
	if (glfwInit() != GLFW_TRUE)
		throw std::runtime_error("Could not initialize GLFW.");
	DebugPrint("Initialized GLFW.\n");
	InitGlfwWindow();
}
void Window::InitGlfwWindow()
{
	if (!isOpen)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		window = glfwCreateWindow(windowSize.x, windowSize.y, windowName.c_str(), nullptr, nullptr);
		if (window == nullptr) throw std::runtime_error("Failed to create a window");
		isOpen = true; DebugPrint("Created window. (hidden)\n");
	}
}


Window::~Window()
{
	glfwTerminate();
}

void Window::DestroyWindow(const VkInstance& instance)
{
	if (isOpen)
	{
		glfwDestroyWindow(window);
		if (instance != VK_NULL_HANDLE)	vkDestroySurfaceKHR(instance, surface, nullptr);
		isOpen = false; DebugPrint("Window & Surface destroyed.\n");
	}
}

void Window::CreateVkSurface(const VkInstance& instance)
{
	VkResult err{};
	if (instance == VK_NULL_HANDLE || glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		if (err == VK_SUCCESS) throw std::runtime_error("failed to create window surface!");
		throw std::runtime_error("failed to create window surface! CheckVkResult");
	}
	DebugPrint("Created window surface\n");
}
