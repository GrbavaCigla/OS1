#pragma once

#include "syscall_c.h"

void* operator new(size_t);

void operator delete(void*) noexcept;
