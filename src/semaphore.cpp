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
	if (value >= (int)n) {
		value -= (int)n;
		return 0;
	}
	Thread::running->waitHeader = {this, n};
	Thread::dispatch();
	return 0;
}

int Semaphore::signal(unsigned n) {
	value += (int)n;
	return 0;
}

void Semaphore::close() { closed = true; }

void Semaphore::cleanup() {
	Scheduler<RoundRobin>::getInstance().forEach([](Thread* t) {
		if (t->waitHeader.semaphore != nullptr && t->waitHeader.semaphore->closed) {
			t->waitHeader.semaphore = nullptr;
			t->waitHeader.needed = 0;
		}
	});
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
