#include "../h/scheduler.hpp"
#include "../h/allocator.hpp"
#include "../h/helper.hpp"
#include "../h/thread.hpp"

namespace kernel {

void Scheduler::add(Thread* thread) {
	ThreadNode* node = (ThreadNode*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(sizeof(ThreadNode)), 'n');
	node->thread = thread;
	insert(readyQueue, node);
}

Thread* Scheduler::next() {
	if (!readyQueue) return nullptr;

	ThreadNode* start = readyQueue;
	ThreadNode* candidate = readyQueue->next;

	do {
		if (!candidate->thread->finished) {
			readyQueue = candidate;
			return candidate->thread;
		}
		candidate = candidate->next;
	} while (candidate != start);

	return nullptr;
}

void Scheduler::sleep(uint64 ticks) {
	ThreadNode* node = detach(readyQueue, readyQueue);

	ThreadNode** pp = &sleepQueue;
	while (*pp && (*pp)->ticks <= ticks) {
		ticks -= (*pp)->ticks;
		pp = &(*pp)->next;
	}
	node->ticks = ticks;
	node->next = *pp;
	if (node->next)
		node->next->ticks -= ticks;
	*pp = node;
}

void Scheduler::tick() {
	if (!sleepQueue)
		return;
	if (sleepQueue->ticks > 0)
		sleepQueue->ticks--;
	while (sleepQueue && sleepQueue->ticks == 0) {
		ThreadNode* node = sleepQueue;
		sleepQueue = node->next;
		insert(readyQueue, node);
	}
}

bool Scheduler::hasPending() {
	if (!readyQueue)
		return false;
	ThreadNode* node = readyQueue;
	do {
		if (node->thread != Thread::running && !node->thread->finished)
			return true;
		node = node->next;
	} while (node != readyQueue);
	return false;
}

void Scheduler::cleanup() {
	if (!readyQueue)
		return;

	ThreadNode* anchor = readyQueue;
	for (ThreadNode* node = anchor->next; node != anchor;) {
		ThreadNode* nextNode = node->next;
		if (node->thread != Thread::running && node->thread->finished) {
			delete node->thread;
			remove(node);
		}
		node = nextNode;
	}
}

void Scheduler::insert(ThreadNode*& ring, ThreadNode* node) {
	if (!ring) {
		node->next = node;
		node->prev = node;
		ring = node;
		return;
	}

	ThreadNode* tail = ring->prev;
	tail->next = node;
	node->prev = tail;
	node->next = ring;
	ring->prev = node;
}

Scheduler::ThreadNode* Scheduler::detach(ThreadNode*& ring, ThreadNode* node) {
	if (node->next == node) {
		ring = nullptr;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		if (ring == node)
			ring = node->next;
	}
	return node;
}

void Scheduler::remove(ThreadNode* node) {
	detach(readyQueue, node);
	MemoryAllocator::getInstance().free((size_t)node);
}

} // namespace kernel
