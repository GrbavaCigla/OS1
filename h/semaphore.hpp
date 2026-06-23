#pragma once

#include "../lib/hw.h"
#include "scheduler.hpp"

namespace kernel {

class Semaphore {
  public:
	Semaphore(unsigned init);

	static void* operator new(size_t, void* p) noexcept { return p; }

	int wait(unsigned n = 1);
	int signal(unsigned n = 1);
	void close();
	void deallocate();
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
