#include "../h/thread.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"

namespace kernel {

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
	Thread* thread = Scheduler::getInstance().current();
	sys::enterUserspace();
	thread->function();
	thread->status = Status::Finished;
}

void Thread::dispatch() {
	Scheduler& scheduler = Scheduler::getInstance();
	Thread* oldThread = scheduler.current();
	scheduler.next();
	Thread* newThread = scheduler.current();
	if (oldThread == newThread || !oldThread || !newThread) {
		return;
	}

	sys::contextSwitch(&oldThread->context, &newThread->context);
}

} // namespace kernel