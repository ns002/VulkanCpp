#pragma once
#include "pch.h"
namespace VWrapper
{
	enum class ShaderType : uint8_t
	{
		Default = 0u,
		VERTEX = 1U,
		FRAGMENT = 2U
	};

	static const char* ShaderTypeToStr(const ShaderType type)
	{
		switch (type)
		{
		case ShaderType::VERTEX: return "vertex";
		case ShaderType::FRAGMENT: return "fragment";
		default: return "default / undefined";
		}
	}
}