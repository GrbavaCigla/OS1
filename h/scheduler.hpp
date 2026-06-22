#pragma once
#include "allocator.hpp"
#include "helper.hpp"
#include "semaphore.hpp"
#include "thread.hpp"

namespace kernel {

struct RoundRobin {};

template <typename> struct IsSchedulingAlgorithm {
	static constexpr bool value = false;
};
template <> struct IsSchedulingAlgorithm<RoundRobin> {
	static constexpr bool value = true;
};

template <typename Algorithm> class Scheduler {
	static_assert(IsSchedulingAlgorithm<Algorithm>::value,
				  "Scheduler<Algorithm>: Algorithm must be a registered "
				  "scheduling-algorithm marker (see IsSchedulingAlgorithm)");

  public:
	static Scheduler& getInstance() {
		static Scheduler instance;
		return instance;
	}

	Scheduler(const Scheduler&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;

	void add(Thread* thread) {
		ThreadNode* node = (ThreadNode*)MemoryAllocator::getInstance().allocate(
			helper::roundUp(sizeof(ThreadNode)));
		node->thread = thread;
		insert(readyQueue, node);
	}

	Thread* next() {
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

	void sleep(uint64 ticks) {
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

	void tick() {
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

	void block(Semaphore* sem) {
		ThreadNode* node = detach(readyQueue, readyQueue);
		node->sem = sem;
		node->next = nullptr;
		if (blockedQueueTail)
			blockedQueueTail->next = node;
		else
			blockedQueue = node;
		blockedQueueTail = node;
	}

	Thread* unblock(Semaphore* sem) {
		ThreadNode* prev = nullptr;
		for (ThreadNode* node = blockedQueue; node; prev = node, node = node->next) {
			if (node->sem != sem)
				continue;
			if (prev)
				prev->next = node->next;
			else
				blockedQueue = node->next;
			if (blockedQueueTail == node)
				blockedQueueTail = prev;
			insert(readyQueue, node);
			return node->thread;
		}
		return nullptr;
	}

	template <typename F>
	void forEach(F func) {
		if (!readyQueue) return;
		ThreadNode* node = readyQueue;
		do {
			func(node->thread);
			node = node->next;
		} while (node != readyQueue);
	}

	void cleanup() {
		if (!readyQueue)
			return;

		ThreadNode* anchor = readyQueue;
		for (ThreadNode* node = anchor->next; node != anchor;) {
			ThreadNode* nextNode = node->next;
			if (node->thread != Thread::running && node->thread->finished) {
				node->thread->deallocate();
				remove(node);
			}
			node = nextNode;
		}
	}

  private:
	struct ThreadNode {
		Thread* thread;
		ThreadNode* next;
		union {
			ThreadNode* prev;
			uint64 ticks;
			Semaphore* sem;
		};
	};

	void insert(ThreadNode*& ring, ThreadNode* node) {
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

	ThreadNode* detach(ThreadNode*& ring, ThreadNode* node) {
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

	void remove(ThreadNode* node) {
		detach(readyQueue, node);
		MemoryAllocator::getInstance().free((size_t)node);
	}

	ThreadNode* readyQueue = nullptr;
	ThreadNode* sleepQueue = nullptr;
	ThreadNode* blockedQueue = nullptr;
	ThreadNode* blockedQueueTail = nullptr;

	Scheduler() {}
	~Scheduler() = default;
};

} // namespace kernel
