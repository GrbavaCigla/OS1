#include "../h/thread.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"

namespace kernel {

Thread* Thread::running = nullptr;

Thread::Thread(Function function, void* arg, void* stack_space)
    : finished(false), stack(nullptr),
      function(function), arg(arg) {
	if (!function)
		return;

	this->stack = (void*)((uint64)stack_space - DEFAULT_STACK_SIZE);
	this->context.sp = (uint64)stack_space;
	this->context.ra = (size_t)(&this->wrapper);
}

Thread::Thread()
	: finished(false), stack(nullptr),
	  function(nullptr), arg(nullptr), context({.ra = 0, .sp = 0}) {}

Thread::~Thread() {
	MemoryAllocator::getInstance().free((size_t)stack);
}

void* Thread::operator new(size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(helper::roundUp(size));
}

void* Thread::operator new[](size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(helper::roundUp(size));
}

void Thread::operator delete(void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
}

void Thread::operator delete[](void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
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
