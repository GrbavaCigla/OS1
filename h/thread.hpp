#pragma once

#include "../lib/hw.h"
#include "syscall_c.hpp"

namespace kernel {
namespace sys {
void exitSupervisor();
}

class Thread {
  public:
	using Function = void (*)(void*);
	using Argument = void*;

	Thread(Function function, Argument arg, void* stack_space, bool privileged = false);
	Thread(Function function, Argument arg, bool privileged = false);
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

	template <bool privileged = false> static void wrapper() {
		Thread* thread = Thread::running;
		if (!privileged)
			sys::exitSupervisor();
		thread->function(thread->arg);
		thread_exit();
	}
};

} // namespace kernel