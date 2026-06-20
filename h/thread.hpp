#pragma once

#include "../lib/hw.h"

namespace kernel {

class Thread {
  public:
	enum class Status { Ready, Blocked, Finished };

	using Function = void (*)();

	Thread(Function function);
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
	Context context;

	static void wrapper();
};

} // namespace kernel