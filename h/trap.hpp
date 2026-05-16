#pragma once
#include "allocator.hpp"
#include "sys.hpp"

namespace kernel::sys {

inline void handleSyscall() {
	sys::SyscallCode code = (sys::SyscallCode)sys::A0::read();
	uint64 ret = 0;
	uint64 args[] = {sys::A1::read()};

	switch (code) {
	case sys::SyscallCode::MemoryAllocate:
		ret = MemoryAllocator::getInstance().allocate(args[0]);
		break;
	case sys::SyscallCode::MemoryFree:
		ret = MemoryAllocator::getInstance().free(args[0]);
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
