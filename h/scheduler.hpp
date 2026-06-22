#pragma once
#include "allocator.hpp"
#include "helper.hpp"
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
		insert(node);
	}

	Thread* next() {
		if (!cursor) return nullptr;

		ThreadNode* start = cursor;
		ThreadNode* candidate = cursor->next;

		do {
			if (candidate->thread->status() == Thread::Status::Ready) {
				cursor = candidate;
				return candidate->thread;
			}
			candidate = candidate->next;
		} while (candidate != start);

		return nullptr;
	}

	void sleep(uint64 ticks) {
		ThreadNode* node = detach(cursor);

		ThreadNode** pp = &head;
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
		if (!head)
			return;
		if (head->ticks > 0)
			head->ticks--;
		while (head && head->ticks == 0) {
			ThreadNode* node = head;
			head = node->next;
			insert(node);
		}
	}

	template <typename F>
	void forEach(F func) {
		if (!cursor) return;
		ThreadNode* node = cursor;
		do {
			func(node->thread);
			node = node->next;
		} while (node != cursor);
	}

	void cleanup() {
		if (!cursor)
			return;

		ThreadNode* anchor = cursor;
		for (ThreadNode* node = anchor->next; node != anchor;) {
			ThreadNode* nextNode = node->next;
			if (node->thread != Thread::running &&
				node->thread->status() == Thread::Status::Finished) {
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
		};
	};

	void insert(ThreadNode* node) {
		if (!cursor) {
			node->next = node;
			node->prev = node;
			cursor = node;
			return;
		}

		ThreadNode* tail = cursor->prev;
		tail->next = node;
		node->prev = tail;
		node->next = cursor;
		cursor->prev = node;
	}

	ThreadNode* detach(ThreadNode* node) {
		if (node->next == node) {
			cursor = nullptr;
		} else {
			node->prev->next = node->next;
			node->next->prev = node->prev;
			if (cursor == node)
				cursor = node->next;
		}
		return node;
	}

	void remove(ThreadNode* node) {
		detach(node);
		MemoryAllocator::getInstance().free((size_t)node);
	}

	ThreadNode* cursor = nullptr;
	ThreadNode* head = nullptr;

	Scheduler() {}
	~Scheduler() = default;
};

} // namespace kernel
