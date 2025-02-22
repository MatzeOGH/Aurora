#pragma once

#include <source_location>

#ifdef AURORA_PLATFORM_WINDOWS
#define HALT() __debugbreak()
#else
#define HALT() __asm__("int $3") // This is the equivalent of __debugbreak() on Linux
#endif


namespace aurora::core
{
	[[noreturn]] void reportAssert(const char* condition, const char* file, int line);
}

#define ASSERT(expr)	\
do{						\
	if (!(expr))		\
	{					\
		aurora::core::reportAssert(#expr, __FILE__, __LINE__);	\
		HALT();			\
	}					\
} while (0)

