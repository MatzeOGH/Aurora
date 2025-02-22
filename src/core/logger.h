#pragma once

#include "types.h"


namespace aurora::core
{
	void init(i32 bufferSize);

	void log(const char* format, ...) noexcept;

} // namespace aurora::core

#define LOG(format, ...) do{ aurora::core::log(format, ##__VA_ARGS__); } while(0)