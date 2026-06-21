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

	Thread* next() {
		while (cursor) {
			ThreadNode* candidate = cursor->next;
			if (candidate->thread->status != Thread::Status::Finished) {
				cursor = candidate;
				return candidate->thread;
			}
			candidate->thread->deallocate();
			remove(candidate);
		}

		return nullptr;
	}

  private:
	struct ThreadNode {
		Thread* thread;
		ThreadNode* next;
		ThreadNode* prev;
	};

	void remove(ThreadNode* node) {
		if (node->next == node) {
			cursor = nullptr;
		} else {
			node->prev->next = node->next;
			node->next->prev = node->prev;
			if (cursor == node)
				cursor = node->next;
		}
		MemoryAllocator::getInstance().free((size_t)node);
	}

	ThreadNode* cursor = nullptr;

	Scheduler() {}
	~Scheduler() = default;
};

} // namespace kernel
