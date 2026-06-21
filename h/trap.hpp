#pragma once
#include "allocator.hpp"
#include "helper.hpp"
#include "scheduler.hpp"
#include "sys.hpp"
#include "thread.hpp"

namespace kernel::sys {

inline void handleSyscall() {
	sys::SyscallCode code = (sys::SyscallCode)sys::A0::read();
	uint64 ret = 0;
	uint64 args[] = {sys::A1::read(), sys::A2::read(), sys::A3::read()};

	switch (code) {
	case sys::SyscallCode::MemoryAllocate:
		ret = MemoryAllocator::getInstance().allocate(args[0]);
		break;
	case sys::SyscallCode::MemoryFree:
		ret = MemoryAllocator::getInstance().free(args[0]);
		break;
	case sys::SyscallCode::ThreadCreate: {
		Thread* thread = (Thread*)MemoryAllocator::getInstance().allocate(helper::roundUp(sizeof(Thread)));
		*thread = Thread((Thread::Function)args[1], (Thread::Argument)args[2]);
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
	}

	sys::A0::write(ret);
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
