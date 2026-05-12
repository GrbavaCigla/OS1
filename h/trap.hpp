#pragma once

#include "../lib/hw.h"
#include "allocator.hpp"

enum class SCause : uint64 {
	SoftwareTimer = (1ul << 63) | 1,
	Hardware = (1ul << 63) | 9,

	IllegalInstruction = 2,
	LoadAccessFault = 5,
	StoreAccessFault = 7,
	UserSyscall = 8,
	SuperSyscall = 9
};

enum class Syscall : uint64 {
	MemoryAllocate = 0x01,
	MemoryFree = 0x02,
};

extern "C" void handleSupervisorTrap();

extern "C" void supervisorTrap();

inline void registerSupervisorTrap() {
	__asm__ volatile("csrw stvec, %0" : : "r"(&supervisorTrap));
	__asm__ volatile("csrs sstatus, 0x02");
}

inline void handleSyscall() {
	Syscall code;
	uint64 ret = 0;
	uint64 args[1];
	__asm__ volatile("mv %0, a0" : "=r"(code));
	__asm__ volatile("mv %0, a1" : "=r"(args[0]));

	switch (code) {
	case Syscall::MemoryAllocate:
		ret = MemoryAllocator::getInstance().allocate(args[0]);
		break;
	case Syscall::MemoryFree:
		ret = MemoryAllocator::getInstance().free(args[0]);
		break;
	}

	__asm__ volatile("mv a0, %0" : : "r"(ret));
}
