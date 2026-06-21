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
	A0::write((uint64)SyscallCode::ThreadCreate);
	A1::write((uint64)handle);
	A2::write((uint64)start_routine);
	A3::write((uint64)arg);
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
}