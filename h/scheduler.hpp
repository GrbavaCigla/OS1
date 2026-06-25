#pragma once
#include "allocator.hpp"
#include "thread.hpp"

namespace kernel {

class Semaphore;
class Logger;

class Scheduler {
	friend class Semaphore;
	friend class Logger;

  public:
	static Scheduler& getInstance() {
		static Scheduler instance;
		return instance;
	}

	Scheduler(const Scheduler&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;

	bool hasPending();
	Thread* next();
	
	void add(Thread* thread);
	void sleep(uint64 ticks);
	void tick();
	
	void cleanup();

  private:
	struct ThreadNode {
		Thread* thread;
		ThreadNode* next;
		union {
			ThreadNode* prev;
			uint64 ticks;
		};
	};

	void insert(ThreadNode*& ring, ThreadNode* node);
	ThreadNode* detach(ThreadNode*& ring, ThreadNode* node);
	void remove(ThreadNode* node);

	ThreadNode* readyQueue = nullptr;
	ThreadNode* sleepQueue = nullptr;

	Scheduler() {}
	~Scheduler() = default;
};

} // namespace kernel
