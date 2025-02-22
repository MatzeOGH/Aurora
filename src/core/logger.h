#pragma once

#include "types.h"
#include "core/string.h"


namespace aurora::core
{
	void init(i32 bufferSize);

	void log(String format, ...) noexcept;

} // namespace aurora::core

#define LOG(format, ...) aurora::core::log(format, ##__VA_ARGS__);