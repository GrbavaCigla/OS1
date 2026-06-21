#pragma once
#include "../lib/console.h"
#include "allocator.hpp"
#include "helper.hpp"
#include "scheduler.hpp"
#include "sys.hpp"
#include "thread.hpp"

namespace kernel::sys {

inline uint64 handleSyscall(uint64 code, uint64* args) {
	sys::SyscallCode syscallCode = (sys::SyscallCode)code;
	uint64 ret = 0;

	switch (syscallCode) {
	case sys::SyscallCode::MemoryAllocate:
		ret = MemoryAllocator::getInstance().allocate(args[0]);
		break;
	case sys::SyscallCode::MemoryFree:
		ret = MemoryAllocator::getInstance().free(args[0]);
		break;
	case sys::SyscallCode::ThreadCreate: {
		Thread* thread = (Thread*)MemoryAllocator::getInstance().allocate(
			helper::roundUp(sizeof(Thread)));
		*thread = Thread((Thread::Function)args[1], (Thread::Argument)args[2],
						 args[3]);
		Scheduler<RoundRobin>::getInstance().add(thread);
		*(Thread**)args[0] = thread;
		break;
	}
	case sys::SyscallCode::ThreadExit:
		Thread::running->status = Thread::Status::Finished;
		Thread::dispatch();
		break;
	case sys::SyscallCode::ThreadDispatch:
		Thread::dispatch();
		break;
	case sys::SyscallCode::ConsoleGetChar:
		ret = (uint64)__getc();
		break;
	case sys::SyscallCode::ConsolePutChar:
		__putc((char)args[0]);
		break;
	}

	return ret;
}

inline void handleTimer() {
	static size_t ticks = 0;
	ticks++;
	if (ticks >= DEFAULT_TIME_SLICE) {
		ticks = 0;
		Thread::dispatch();
	}
}

} // namespace kernel::sys
