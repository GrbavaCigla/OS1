#pragma once

#include "../lib/hw.h"
#include "scheduler.hpp"

namespace kernel {

class Logger;

class Semaphore {
	friend class Logger;

  public:
	Semaphore(unsigned init);
	~Semaphore();

	static void* operator new(size_t);
	static void* operator new[](size_t);
	static void operator delete(void*) noexcept;
	static void operator delete[](void*) noexcept;

	int wait(unsigned n = 1);
	int signal(unsigned n = 1);
	void close();
	static void cleanup();

	int value;
	bool closed;

  private:
	Scheduler<RoundRobin>::ThreadNode* threads;

	static Semaphore* head;
	Semaphore* next;
	Semaphore* prev;
};

} // namespace kernel
