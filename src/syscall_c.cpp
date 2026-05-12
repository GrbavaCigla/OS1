#include "../h/syscall_c.h"
#include "../h/helper.hpp"

extern "C" {
void* mem_alloc(size_t size) {
	uint64 ret;
    size = ROUND_UP(size, MEM_BLOCK_SIZE);

	__asm__ volatile("li a0, 1\n"
					 "mv a1, %1\n"
					 "ecall\n"
					 "mv %0, a0"
					 : "=r"(ret)
					 : "r"(size));
	return (void*)ret;
}

int mem_free(void* ptr) {
	uint64 ret;
	__asm__ volatile("li a0, 2\n"
					 "mv a1, %1\n"
					 "ecall\n"
					 "mv %0, a0"
					 : "=r"(ret)
					 : "r"(ptr));
	return (int)ret;
}
}