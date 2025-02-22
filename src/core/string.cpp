#include "string.h"
#include "arena.h"

#include <cstring>

Aurora::String::String(Arena* arena, const char* cstring) noexcept
{
	size = (i32)strlen(cstring);
	data = arena->alloc<char>(size + 1);
	memcpy(data, cstring, size);
}
