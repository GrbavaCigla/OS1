#include "../h/semaphore.hpp"
#include "../h/allocator.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"

namespace kernel {

Semaphore* Semaphore::head = nullptr;

Semaphore::Semaphore(unsigned init)
	: value((int)init), closed(false), next(nullptr), prev(nullptr) {
	if (head)
		head->prev = this;
	next = head;
	head = this;
}

int Semaphore::wait(unsigned n) {
	if (closed) return -1;
	value -= (int)n;
	if (value < 0) {
		Scheduler<RoundRobin>::getInstance().block(this);
		Thread::dispatch();
	}
	return closed ? -1 : 0;
}

int Semaphore::signal(unsigned n) {
	for (unsigned i = 0; i < n; i++) {
		value++;
		if (value <= 0)
			Scheduler<RoundRobin>::getInstance().unblock(this);
	}
	return 0;
}

void Semaphore::close() {
	closed = true;
	while (Scheduler<RoundRobin>::getInstance().unblock(this))
		;
}

void Semaphore::cleanup() {
	for (Semaphore* sem = head; sem != nullptr;) {
		Semaphore* next = sem->next;
		if (sem->closed)
			sem->deallocate();
		sem = next;
	}
}

void Semaphore::deallocate() {
	if (prev)
		prev->next = next;
	else
		head = next;
	if (next)
		next->prev = prev;

	MemoryAllocator::getInstance().free((size_t)this);
}

} // namespace kernel
