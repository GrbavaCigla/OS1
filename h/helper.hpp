#pragma once

#include "../lib/hw.h"

namespace kernel::helper {

template <typename T> constexpr T alignUp(T x, T size) {
	return (x + size - 1) & ~(size - 1);
}

template <typename T> constexpr T roundUp(T n) { return (n + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE; }

} // namespace kernel::helper
