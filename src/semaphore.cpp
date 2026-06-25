#include "../h/semaphore.hpp"
#include "../h/allocator.hpp"
#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/thread.hpp"

namespace kernel {

Semaphore* Semaphore::head = nullptr;

Semaphore::Semaphore(unsigned init)
	: value((int)init), closed(false), threads(nullptr), threadsTail(nullptr),
	  next(nullptr), prev(nullptr) {
	if (head)
		head->prev = this;
	next = head;
	head = this;
}

void Semaphore::insert(Scheduler::ThreadNode* node) {
	node->next = nullptr;
	if (threadsTail)
		threadsTail->next = node;
	else
		threads = node;
	threadsTail = node;
}

Scheduler::ThreadNode* Semaphore::detach() {
	Scheduler::ThreadNode* node = threads;
	threads = node->next;
	if (!threads)
		threadsTail = nullptr;
	return node;
}

int Semaphore::wait(unsigned n) {
	if (closed)
		return -1;

	if (value >= (int)n) {
		value -= (int)n;
		return 0;
	}

	Scheduler& sched = Scheduler::getInstance();
	Scheduler::ThreadNode* node =
		sched.detach(sched.readyQueue, sched.readyQueue);
	node->tokens = n;
	insert(node);

	Thread::dispatch();

	return closed ? -1 : 0;
}

int Semaphore::signal(unsigned n) {
	if (closed)
		return -1;
	Scheduler& sched = Scheduler::getInstance();
	value += (int)n;

	while (threads && value >= (int)threads->tokens) {
		Scheduler::ThreadNode* node = detach();
		value -= (int)node->tokens;
		sched.insert(sched.readyQueue, node);
	}
	return 0;
}

void Semaphore::close() {
	closed = true;
	Scheduler& sched = Scheduler::getInstance();
	while (threads)
		sched.insert(sched.readyQueue, detach());
}

void Semaphore::cleanup() {
	for (Semaphore* sem = head; sem != nullptr;) {
		Semaphore* next = sem->next;
		if (sem->closed)
			delete sem;
		sem = next;
	}
}

Semaphore::~Semaphore() {
	if (prev)
		prev->next = next;
	else
		head = next;
	if (next)
		next->prev = prev;
}

void* Semaphore::operator new(size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(size), 's');
}

void* Semaphore::operator new[](size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(size), 'S');
}

void Semaphore::operator delete(void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
}

void Semaphore::operator delete[](void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
}

} // namespace kernel
