#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../lib/hw.h"

void* mem_alloc(size_t);

int mem_free(void*);

#ifdef __cplusplus
}
#endif