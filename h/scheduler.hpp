#pragma once
#include "thread.hpp"

namespace kernel {

class Scheduler {
  public:
	static Scheduler& getInstance() {
		static Scheduler instance;
		return instance;
	}

	Scheduler(const Scheduler&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;

	void add(Thread*);
	Thread* current();
	void next();

  private:
	struct ThreadNode {
		Thread* thread;
		ThreadNode* next;
		ThreadNode* prev;
	};

	ThreadNode* head;

	Scheduler() {}
	~Scheduler() = default;
};

} // namespace kernel