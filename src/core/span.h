#pragma once

#include "types.h"
#include "asserts.h"

#include <initializer_list>



// Span is a none owning view of a contiguous sequence of objects
template<typename T>
struct Span
{
	const T* data{};
	const i32 size{};

	// constructors
	constexpr Span() noexcept = default;
	constexpr Span(NullptrType) noexcept {}
	constexpr Span(std::initializer_list<T> list) noexcept : data(list.begin()), size((i32)list.size()) {}

	constexpr ~Span() noexcept = default;

	inline const T& operator[](i32 index) const noexcept
	{
		return data[index];
	}

	inline constexpr const T* begin() const noexcept
	{
		return data;
	}

	inline constexpr const T* end() const noexcept
	{
		return data + size;
	}
};