

// see https://gist.github.com/rygorous/2156668

#pragma once

#include <cstdint>

typedef struct half
{
	unsigned short sh;

	half() : sh(0) {}
	half(const float x);
	half(const half& other);
	operator float() const;

	half& operator=(const half& other)
	{
		sh = other.sh;
		return *this;
	}

} half;

extern short to_half_float(const float x);
extern float from_half_float(const short x);