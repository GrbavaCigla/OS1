#pragma once

#include "../lib/hw.h"

namespace kernel {

class Semaphore;

class Thread {
  public:
	enum class Status { Ready, Blocked, Finished };

	using Function = void (*)(void*);
	using Argument = void*;

	Thread(Function function, Argument arg, void* stack_space);
	Thread();

	Status status() const;

	bool finished;

	struct WaitHeader {
		Semaphore* semaphore;
		unsigned needed;
	} waitHeader;

	struct Context {
		uint64 ra;
		uint64 sp;
	};

	static Thread* running;

	static void dispatch();

	void deallocate();

  private:
	void* stack;
	Function function;
	Argument arg;
	Context context;

	static void wrapper();
};

} // namespace kernel