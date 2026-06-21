#pragma once

#include "../lib/hw.h"

namespace kernel {

class Thread {
  public:
	enum class Status { Ready, Blocked, Finished };

	using Function = void (*)(void*);
	using Argument = void*;

	Thread(Function function, Argument arg, uint64 stack_space);
	Thread();

	Status status;

	struct Context {
		uint64 ra;
		uint64 sp;
	};

	static Thread* running;

	static void dispatch();

  private:
	void* stack;
	Function function;
	Argument arg;
	Context context;

	static void wrapper();
};

} // namespace kernel