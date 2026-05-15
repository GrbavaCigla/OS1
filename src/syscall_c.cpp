#include "../h/syscall_c.h"
#include "../h/helper.hpp"
#include "../h/sys.hpp"

extern "C" {

using namespace kernel::sys;
using namespace kernel::helper;

void* mem_alloc(size_t size) {
	size = round_up(size, MEM_BLOCK_SIZE);

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
}