#pragma once

#include "allocator.hpp"
#include "helper.hpp"
#include "semaphore.hpp"

namespace kernel {

template <unsigned int capacity = 4>
class Buffer {
  public:
	Buffer() : available(0) {}

	unsigned int getCount() const { return count; }

	void put(char c) {
		data[tail] = c;
		tail = (tail + 1) % capacity;
		count++;
		available.signal();
	}

	char get() {
		available.wait();
		char c = data[head];
		head = (head + 1) % capacity;
		count--;
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

	bool isEmpty() const { return count == 0; }
	bool isFull() const { return count == capacity; }

  private:
	char data[capacity];
	int head = 0;
	int tail = 0;
	unsigned int count = 0;
	Semaphore available;
};

} // namespace kernel
