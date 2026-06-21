#pragma once

#include "../lib/hw.h"

namespace kernel {

class Thread {
  public:
	enum class Status { Ready, Blocked, Finished };

	using Function = void (*)(void*);

	Thread(Function function, void* arg);
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
	void* arg;
	Context context;

	static void wrapper();
};

} // namespace kernel