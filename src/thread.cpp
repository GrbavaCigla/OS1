#include "../h/thread.hpp"
#include "../h/allocator.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"

namespace kernel {

Thread* Thread::running = nullptr;
uint64 Thread::ticks = 0;

Thread::Thread(Function function, void* arg, void* stack_space, bool privileged)
	: finished(false), stack(nullptr), function(function), arg(arg) {
	if (!function)
		return;

	this->stack = (void*)((uint64)stack_space - DEFAULT_STACK_SIZE);
	this->context.sp = (uint64)stack_space;
	if (privileged)
		this->context.ra = (size_t)(&Thread::wrapper<true>);
	else
		this->context.ra = (size_t)(&Thread::wrapper<false>);
}

Thread::Thread(Function function, void* arg, bool privileged)
	: finished(false), stack(nullptr), function(function), arg(arg) {
	if (!function)
		return;

	size_t allocated = MemoryAllocator::getInstance().allocate(
		helper::roundUp(DEFAULT_STACK_SIZE), 'c');
	this->stack = (void*)allocated;
	this->context.sp = allocated + DEFAULT_STACK_SIZE;
	if (privileged)
		this->context.ra = (size_t)(&Thread::wrapper<true>);
	else
		this->context.ra = (size_t)(&Thread::wrapper<false>);
}

Thread::Thread()
	: finished(false), stack(nullptr), function(nullptr), arg(nullptr),
	  context({.ra = 0, .sp = 0}) {}

Thread::~Thread() { MemoryAllocator::getInstance().free((size_t)stack); }

void* Thread::operator new(size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(size), 't');
}

void* Thread::operator new[](size_t size) {
	return (void*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(size), 'T');
}

void Thread::operator delete(void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
}

void Thread::operator delete[](void* ptr) noexcept {
	MemoryAllocator::getInstance().free((size_t)ptr);
}

void Thread::dispatch() {
	Thread* oldThread = Thread::running;
	Thread* newThread = Scheduler::getInstance().next();

	if (!newThread || newThread == oldThread)
		return;

	ticks = 0;
	Thread::running = newThread;
	sys::contextSwitch(&oldThread->context, &newThread->context);
}

} // namespace kernel
