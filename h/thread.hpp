#pragma once

#include "../lib/hw.h"

namespace kernel {

class Thread {
  public:
	using Function = void (*)(void*);
	using Argument = void*;

	Thread(Function function, Argument arg, void* stack_space);
	Thread();
	~Thread();

	static void* operator new(size_t);
	static void* operator new[](size_t);
	static void operator delete(void*) noexcept;
	static void operator delete[](void*) noexcept;

	bool finished;

	struct Context {
		uint64 ra;
		uint64 sp;
	};

	static Thread* running;

	static uint64 ticks;

	static void dispatch();

  private:
	void* stack;
	Function function;
	Argument arg;
	Context context;

	static void wrapper();
};

} // namespace kernel