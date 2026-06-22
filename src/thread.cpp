#include "../h/thread.hpp"
#include "../h/semaphore.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"

namespace kernel {

Thread* Thread::running = nullptr;

Thread::Thread(Function function, void* arg, void* stack_space)
    : finished(false), waitHeader({nullptr, 0}), stack(nullptr),
      function(function), arg(arg) {
	if (!function)
		return;

	this->stack = (void*)((uint64)stack_space - DEFAULT_STACK_SIZE);
	this->context.sp = (uint64)stack_space;
	this->context.ra = (size_t)(&this->wrapper);
}

Thread::Thread()
	: finished(false), waitHeader({nullptr, 0}), stack(nullptr),
	  function(nullptr), arg(nullptr), context({.ra = 0, .sp = 0}) {}

Thread::Status Thread::status() const {
	if (finished) return Status::Finished;
	if (waitHeader.semaphore != nullptr &&
	    waitHeader.semaphore->value < (int)waitHeader.needed)
		return Status::Blocked;
	return Status::Ready;
}

void Thread::deallocate() {
	MemoryAllocator::getInstance().free((size_t)stack);
	MemoryAllocator::getInstance().free((size_t)this);
}

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

	Thread::running = newThread;
	sys::contextSwitch(&oldThread->context, &newThread->context);
}

} // namespace kernel
