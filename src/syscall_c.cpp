#include "../h/syscall_c.hpp"
#include "../h/helper.hpp"
#include "../h/sys.hpp"

extern "C" {

using namespace kernel::sys;
using namespace kernel::helper;

void* mem_alloc(size_t size) {
	size = roundUp(size);

	A0::write((uint64)SyscallCode::MemoryAllocate);
	A1::write(size);
	ecall();
	return (void*)A0::read();
}

int mem_free(void* ptr) {
	A0::write((uint64)SyscallCode::MemoryFree);
	A1::write((uint64)ptr);
	ecall();
	return (int)A0::read();
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
	uint64 stack_space = (uint64)mem_alloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;

	A0::write((uint64)SyscallCode::ThreadCreate);
	A1::write((uint64)handle);
	A2::write((uint64)start_routine);
	A3::write((uint64)arg);
	A4::write((uint64)stack_space);
	ecall();
	return (int)A0::read();
}

int thread_exit() {
	A0::write((uint64)SyscallCode::ThreadExit);
	ecall();
	return A0::read();
}

void thread_dispatch() {
	A0::write((uint64)SyscallCode::ThreadDispatch);
	ecall();
}

int time_sleep(time_t period) {
	A0::write((uint64)SyscallCode::ThreadSleep);
	A1::write((uint64)period);
	ecall();
	return (int)A0::read();
}

int sem_open(sem_t* handle, unsigned init) {
	A0::write((uint64)SyscallCode::SemaphoreOpen);
	A1::write((uint64)handle);
	A2::write((uint64)init);
	ecall();
	return (int)A0::read();
}

int sem_close(sem_t handle) {
	A0::write((uint64)SyscallCode::SemaphoreClose);
	A1::write((uint64)handle);
	ecall();
	return (int)A0::read();
}

int sem_wait(sem_t handle) {
	A0::write((uint64)SyscallCode::SemaphoreWait);
	A1::write((uint64)handle);
	ecall();
	return (int)A0::read();
}

int sem_signal(sem_t handle) {
	A0::write((uint64)SyscallCode::SemaphoreSignal);
	A1::write((uint64)handle);
	ecall();
	return (int)A0::read();
}

int sem_wait_n(sem_t handle, unsigned n) {
	A0::write((uint64)SyscallCode::SemaphoreWaitN);
	A1::write((uint64)handle);
	A2::write((uint64)n);
	ecall();
	return (int)A0::read();
}

int sem_signal_n(sem_t handle, unsigned n) {
	A0::write((uint64)SyscallCode::SemaphoreSignalN);
	A1::write((uint64)handle);
	A2::write((uint64)n);
	ecall();
	return (int)A0::read();
}

char getc() {
	A0::write((uint64)SyscallCode::ConsoleGetChar);
	ecall();
	return (char)A0::read();
}

void putc(char chr) {
	A0::write((uint64)SyscallCode::ConsolePutChar);
	A1::write((uint64)chr);
	ecall();
}
}