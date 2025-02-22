#include "logger.h"

#include <stdio.h>
#include <stdarg.h>

namespace Aurora
{

	void init(i32 bufferSize)
	{

	}

	void log(String format, ...) noexcept
	{
		char buffer[4096] = { };
		char* ptr = buffer;

		va_list args;
		va_start(args, format);
		int n = vsnprintf(ptr, sizeof(buffer), format.data, args);
		va_end(args);

		buffer[n] = '\n';
		buffer[n + 1] = '\0';

		fwrite(buffer, sizeof(char), n+1, stdout);
		fflush(stdout);
	}
} // namespace aurora