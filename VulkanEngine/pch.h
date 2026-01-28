#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
//#include <crtdbg.h>
#include <iostream>
#include <optional>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <typeinfo>
#include <vector>

#include "vec.h"

//include vulkan first
//or use this #define and let glfw handle it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //for spinny spinny

//try removing these globals
static bool exitRequest = false;
constexpr size_t sleepTime = 0U;	//how long to sleep for after application exits
constexpr vec2<uint16_t> windowSize = { 800U, 600U };

//try to prevent it from printing every frame, do that with cout or printf
void DebugPrint(const char* str, std::optional<char> suffix = std::nullopt) noexcept(false);
//try to prevent it from printing every frame, do that with cout or printf
void DebugPrint(const char** str_arr, const size_t& size = 1, std::optional<const char*> seperator = " ", std::optional<char> suffix = std::nullopt) noexcept(false);
//try to prevent it from printing every frame, do that with cout or printf
void DebugPrint(const std::string& str, std::optional<char> suffix = std::nullopt) noexcept;
//try to prevent it from printing every frame, do that with cout or printf
void DebugPrint(const std::string* str_arr, const size_t& size = 1, std::optional<const char*> seperator = " ", std::optional<char> suffix = std::nullopt) noexcept(false);

//puts this thread to sleep for (s) seconds
inline void SleepS(const size_t s)   { std::this_thread::sleep_for(std::chrono::seconds(s)); }
//puts this thread to sleep for (ms) milliseconds
inline void SleepMS(const size_t ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
//puts this thread to sleep for (μs) microseconds
inline void SleepUS(const size_t μs) { std::this_thread::sleep_for(std::chrono::microseconds(μs)); }
bool CStringsEqual(const char* cstring1, const char* cstring2) noexcept;

enum class BuildConfiguration : uint8_t
{
	Debug, Release, Distro = 0xff
};

#if defined(DEBUG_TOOLS) && defined(NDEBUG)
constexpr BuildConfiguration buildConfig = BuildConfiguration::Release;
#elif DEBUG_TOOLS
constexpr BuildConfiguration buildConfig = BuildConfiguration::Debug;
#else
constexpr BuildConfiguration buildConfig = BuildConfiguration::Distro;
#endif