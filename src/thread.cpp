#include "../h/thread.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"

namespace kernel {

Thread* Thread::running = nullptr;

Thread::Thread(Function function, void* arg)
    : status(Status::Ready), function(function), arg(arg) {
	if (!function)
		return;

	this->stack =
		(void*)MemoryAllocator::getInstance().allocate(DEFAULT_STACK_SIZE);
	this->context.sp = (uint64)stack + DEFAULT_STACK_SIZE * MEM_BLOCK_SIZE;
	this->context.ra = (size_t)(&this->wrapper);
}

Thread::Thread()
	: status(Status::Finished), stack(nullptr), function(nullptr), arg(nullptr),
	  context({.ra = 0, .sp = 0}) {}

void Thread::wrapper() {
	Thread* thread = Thread::running;
	sys::exitSupervisor();
	thread->function(thread->arg);
	thread_exit();
}

void Thread::dispatch() {
	Thread* oldThread = Thread::running;
	Thread* newThread = Scheduler<RoundRobin>::getInstance().next();

	if (!newThread || newThread == oldThread)
		return;
	if (newThread->status == Status::Finished)
		return;

	Thread::running = newThread;
	sys::contextSwitch(&oldThread->context, &newThread->context);
}

} // namespace kernel
