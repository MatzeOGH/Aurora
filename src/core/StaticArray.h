#pragma once

#include "core/types.h"

namespace Aurora {

	template<typename N, i32 N>
	struct StaticArray
	{
		T data[N];

		T& operator[](i32 i) { return data[i];  }
		const T& operator[](i32 i) const { return data[i]; }

		T* begin() const { return data; }
		T* end() const { return data + N; }
	};

} // Aurora