#pragma once

#include "types.h"
#include "core/string.h"


namespace Aurora
{
	void init(i32 bufferSize);

	void log(String format, ...) noexcept;

} // namespace aurora::core

#define LOG(format, ...) Aurora::log(format, ##__VA_ARGS__);