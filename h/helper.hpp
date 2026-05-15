#pragma once

#include "../lib/console.h"
#include "../lib/hw.h"

namespace kernel::helper {

template <typename T> constexpr T alignUp(T x, T size) {
	return (x + size - 1) & ~(size - 1);
}

template <typename T> constexpr T roundUp(T n, T d) { return (n + d - 1) / d; }

void print(const char*);

template <typename T> void print(T x) {
	uint64 b = 0;
	do {
		b *= 10;
		b += x % 10;
		x /= 10;
	} while (x != 0);
	do {
		__putc(b % 10 + '0');
		b /= 10;
	} while (b != 0);
}

} // namespace kernel::helper
