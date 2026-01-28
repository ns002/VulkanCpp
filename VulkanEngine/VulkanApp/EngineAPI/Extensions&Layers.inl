#pragma once
#include "pch.h"

#ifdef DEBUG_TOOLS
//this can be used to create an array of c_strings
#define VALIDATION_LAYERS "VK_LAYER_KHRONOS_validation"//,
#else
#define VALIDATION_LAYERS //nothing
#endif
//this can be used to create an array of c_strings
#define DEVICE_EXTENSIONS VK_KHR_SWAPCHAIN_EXTENSION_NAME//,

static const std::vector<const char*> validationLayers = { VALIDATION_LAYERS }, deviceExtensions = { DEVICE_EXTENSIONS };