#pragma once 

#include "memory.h"
#include "asserts.h"

#include <cstdlib> // malloc, free
#include <cstddef> // std::ptrdiff_t


// TODO: remove memory management(new free) form the arena
namespace Aurora {


struct Arena {
	char* begin{};
	char* head{};
	char* end{};

	static Arena makeArena(std::ptrdiff_t size) {
		Arena arena;
		arena.begin = (char*)malloc(size);
		arena.end = arena.begin + size;
		arena.head = arena.end;
		return arena;
	}

	static void destroyArena(Arena& arena) {
		free(arena.begin);
		arena = {};
	}

	template<typename T>
	T* alloc(std::ptrdiff_t count = 1) {

		ASSERT(count > 0);
		constexpr std::ptrdiff_t size = sizeof(T);
		constexpr std::ptrdiff_t align = alignof(T);

		std::ptrdiff_t pad = (uintptr_t)head & (align - 1);

		//assert when out of memory
		ASSERT(count < (head - begin - pad) / size);
		head -= size * count + pad;
		T* r = (T*)head;
		
		for (i32 i = 0; i < count; i++) {
			PLACEMENT_NEW(r + i) T {};
		}

		return r;
	}

	void reset() {
		head = end;
	}
};

}
