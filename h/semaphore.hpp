#pragma once

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
	void insert(Scheduler::ThreadNode* node);
	Scheduler::ThreadNode* detach();

	Scheduler::ThreadNode* threads;
	Scheduler::ThreadNode* threadsTail;

	static Semaphore* head;
	Semaphore* next;
	Semaphore* prev;
};

} // namespace kernel
