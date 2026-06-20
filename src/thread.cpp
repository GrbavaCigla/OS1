#include "../h/thread.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"

namespace kernel {

Thread* Thread::running = nullptr;

Thread::Thread(Function function) : status(Status::Ready), function(function) {
	if (!function)
		return;

	this->stack =
		(void*)MemoryAllocator::getInstance().allocate(DEFAULT_STACK_SIZE);
	this->context.sp = (uint64)stack + DEFAULT_STACK_SIZE * MEM_BLOCK_SIZE;
	this->context.ra = (size_t)(&this->wrapper);
}

Thread::Thread()
	: status(Status::Finished), stack(nullptr), function(nullptr),
	  context({.ra = 0, .sp = 0}) {}

void Thread::wrapper() {
	Thread* thread = Thread::running;
	sys::enterUserspace();
	thread->function();
	thread->status = Status::Finished;
	Thread::dispatch();
	while (true) {
		Thread::dispatch();
	}
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
