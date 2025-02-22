#pragma once

// unsigend types
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

// signed types
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

// floats
using f32 = float;
using f64 = double;

using NullptrType = decltype(nullptr); // taken from <cstddef>

using ptrdiff = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
using addptr = ptrdiff; // should be the same as ptr diff


