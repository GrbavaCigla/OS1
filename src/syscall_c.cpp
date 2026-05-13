#include "../h/syscall_c.h"
#include "../h/helper.hpp"
#include "../h/sys.hpp"

extern "C" {
void* mem_alloc(size_t size) {
	size = Kernel::Helper::round_up(size, MEM_BLOCK_SIZE);

	Kernel::Sys::A0::write((uint64)Kernel::Sys::SyscallCode::MemoryAllocate);
	Kernel::Sys::A1::write(size);
	Kernel::Sys::ecall();
	return (void*)Kernel::Sys::A0::read();
}

int mem_free(void* ptr) {
	Kernel::Sys::A0::write((uint64)Kernel::Sys::SyscallCode::MemoryFree);
	Kernel::Sys::A1::write((uint64)ptr);
	Kernel::Sys::ecall();
	return (int)Kernel::Sys::A0::read();
}
}