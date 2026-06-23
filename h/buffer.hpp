#pragma once

#include "../lib/hw.h"
#include "allocator.hpp"
#include "helper.hpp"
#include "semaphore.hpp"

namespace kernel {

enum class BufferType { Input, Output };

template <BufferType T>
class Buffer {
  public:
	Buffer() : available(T == BufferType::Output ? capacity : 0) {}

	bool put(char c) {
		if (T == BufferType::Output)
			available.wait();
		else if (isFull())
			return false;

		data[tail] = c;
		tail = (tail + 1) % capacity;

		if (T == BufferType::Input)
			available.signal();
		return true;
	}

	char get() {
		if (T == BufferType::Input)
			available.wait();

		char c = data[head];
		head = (head + 1) % capacity;

		if (T == BufferType::Output)
			available.signal();
		return c;
	}

	static void* operator new(size_t size) {
		return (void*)MemoryAllocator::getInstance().allocate(helper::roundUp(size));
	}
	static void* operator new[](size_t size) {
		return (void*)MemoryAllocator::getInstance().allocate(helper::roundUp(size));
	}
	static void operator delete(void* ptr) noexcept {
		MemoryAllocator::getInstance().free((size_t)ptr);
	}
	static void operator delete[](void* ptr) noexcept {
		MemoryAllocator::getInstance().free((size_t)ptr);
	}

	bool isEmpty() const {
		if (T == BufferType::Input)
			return available.value == 0;
		else
			return available.value == capacity;
	}
	bool isFull() const {
		if (T == BufferType::Input)
			return available.value == capacity;
		else
			return available.value == 0;
	}

  private:
	static const int capacity = 256;

	char data[capacity];
	int head = 0;
	int tail = 0;
	Semaphore available;
};

using InputBuffer = Buffer<BufferType::Input>;
using OutputBuffer = Buffer<BufferType::Output>;

} // namespace kernel
