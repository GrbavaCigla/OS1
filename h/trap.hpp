#pragma once
#include "allocator.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "semaphore.hpp"
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
		Thread* thread = new Thread((Thread::Function)args[1],
									(Thread::Argument)args[2], (void*)args[3]);
		Scheduler<RoundRobin>::getInstance().add(thread);
		*(Thread**)args[0] = thread;
		break;
	}
	case sys::SyscallCode::ThreadExit:
		Thread::running->finished = true;
		Thread::dispatch();
		break;
	case sys::SyscallCode::ThreadDispatch:
		Thread::dispatch();
		break;
	case sys::SyscallCode::ThreadSleep:
		Scheduler<RoundRobin>::getInstance().sleep(args[0]);
		Thread::dispatch();
		break;
	case sys::SyscallCode::SemaphoreOpen: {
		Semaphore* sem = new Semaphore((unsigned)args[1]);
		*(Semaphore**)args[0] = sem;
		break;
	}
	case sys::SyscallCode::SemaphoreClose:
		((Semaphore*)args[0])->close();
		break;
	case sys::SyscallCode::SemaphoreWait:
		ret = (uint64)((Semaphore*)args[0])->wait(1);
		break;
	case sys::SyscallCode::SemaphoreSignal:
		ret = (uint64)((Semaphore*)args[0])->signal(1);
		break;
	case sys::SyscallCode::SemaphoreWaitN:
		ret = (uint64)((Semaphore*)args[0])->wait((unsigned)args[1]);
		break;
	case sys::SyscallCode::SemaphoreSignalN:
		ret = (uint64)((Semaphore*)args[0])->signal((unsigned)args[1]);
		break;
	case sys::SyscallCode::ConsoleGetChar:
		ret = (uint64)console::inputBuffer->get();
		break;
	case sys::SyscallCode::ConsolePutChar:
		if (console::outputBuffer->isFull()) {
			console::flushOutput();
		}
		console::outputBuffer->put(args[0]);
		break;
	}

	return ret;
}

inline void handleTimer() {
	Scheduler<RoundRobin>::getInstance().tick();
	Thread::ticks++;
	if (Thread::ticks >= DEFAULT_TIME_SLICE) {
		Scheduler<RoundRobin>::getInstance().cleanup();
		// Semaphore::cleanup();
		Thread::dispatch();
	}
}

inline void handleHardware() {
	int irq = plic_claim();
	if (irq == (int)CONSOLE_IRQ)
		console::bufferInput();
	if (irq)
		plic_complete(irq);
}

} // namespace kernel::sys
