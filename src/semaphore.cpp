#include "../h/semaphore.hpp"
#include "../h/allocator.hpp"
#include "../h/thread.hpp"

namespace kernel {

Semaphore* Semaphore::head = nullptr;

Semaphore::Semaphore(unsigned init)
	: value((int)init), next(nullptr), prev(nullptr) {
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
