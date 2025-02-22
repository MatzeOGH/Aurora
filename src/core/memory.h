#pragma once

#include "types.h"

// allocate on the stack
#define ALLOCA(TYPE, COUNT) (TYPE*)alloca((COUNT) * sizeof(TYPE))

//https://wunkolo.github.io/post/2022/02/memory-size-literals/
constexpr i64 operator""_KiB(unsigned long long int x) noexcept { return 1024ULL * x; }
constexpr i64 operator""_MiB(unsigned long long int x) noexcept { return 1024_KiB * x; }
constexpr i64 operator""_GiB(unsigned long long int x) noexcept { return 1024_MiB * x; }
constexpr i64 operator""_TiB(unsigned long long int x) noexcept { return 1024_GiB * x; }

// custom placement new tag to not include <new>
struct NewTag {};
inline void* operator new(u64, NewTag, void* ptr) noexcept { return ptr; }
inline void operator delete(void*, NewTag, void*) noexcept {}
#define PLACEMENT_NEW(ptr) new (NewTag{}, ptr)
