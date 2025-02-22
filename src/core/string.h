#pragma once

#include "types.h"

#include "cstring" // memcmp

//TODO: implement a string view class
namespace Aurora
{
	struct String {
		char* data{};
		i32 size{};

		String() noexcept = default;

		explicit String(struct Arena*, const char* cstring) noexcept;

		template<i32 N>
		String(const char(&str)[N]) noexcept
			: data{ (char*)str }
			, size{ N - 1 }
		{
		}

		inline bool operator==(String other) const noexcept
		{
			return size == other.size && (!size || !memcmp(data, other.data, size));
		}


	};
} // Aurora::Core

