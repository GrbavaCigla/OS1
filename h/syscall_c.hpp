#pragma once

#include "../lib/hw.h"

namespace kernel {
class Thread;
}

typedef kernel::Thread* thread_t;

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t);

int mem_free(void*);

int thread_create(thread_t*, void (*)(void*), void*);

int thread_exit();

void thread_dispatch();

char getc();

void putc(char);

#ifdef __cplusplus
}
#endif