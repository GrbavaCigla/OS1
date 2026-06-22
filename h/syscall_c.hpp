#pragma once

#include "../lib/hw.h"

namespace kernel {
class Thread;
class Semaphore;
}

typedef kernel::Thread* thread_t;
typedef kernel::Semaphore* sem_t;

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t);

int mem_free(void*);

int thread_create(thread_t*, void (*)(void*), void*);

int thread_exit();

void thread_dispatch();

int sem_open(sem_t*, unsigned);

int sem_close(sem_t);

int sem_wait(sem_t);

int sem_signal(sem_t);

int sem_wait_n(sem_t, unsigned);

int sem_signal_n(sem_t, unsigned);

char getc();

void putc(char);

#ifdef __cplusplus
}
#endif