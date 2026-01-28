#pragma once
#include <sstream>

static class DebugLog
{
	std::stringstream ss;

public:
	DebugLog() = default;
	~DebugLog() noexcept;

	const char* Append(const char* str) noexcept;

} logger;