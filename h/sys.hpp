#pragma once

#include "../h/allocator.hpp"
#include "../lib/hw.h"

#define REGISTER_ACCESSOR(name, reg)                                           \
	struct name {                                                              \
		static inline uint64 read() {                                          \
			uint64 val;                                                        \
			__asm__ volatile("mv %0, " #reg : "=r"(val));                      \
			return val;                                                        \
		}                                                                      \
		static inline void write(uint64 val) {                                 \
			__asm__ volatile("mv " #reg ", %0" : : "r"(val));                  \
		}                                                                      \
	};

#define STATUS_ACCESSOR(name, reg, type, bittype)                              \
	struct name {                                                              \
		static inline type read() {                                            \
			type val;                                                          \
			__asm__ volatile("csrr %0, " #reg : "=r"(val));                    \
			return val;                                                        \
		}                                                                      \
		static inline void write(type val) {                                   \
			__asm__ volatile("csrw " #reg ", %0" : : "r"(val));                \
		}                                                                      \
		static inline void set(bittype bit) {                                  \
			__asm__ volatile("csrs " #reg ", %0" : : "r"(bit));                \
		}                                                                      \
		static inline void clear(bittype bit) {                                \
			__asm__ volatile("csrc " #reg ", %0" : : "r"(bit));                \
		}                                                                      \
	};

extern "C" void supervisorTrap();

extern "C" void handleSupervisorTrap();

namespace Kernel::Sys {

enum class SCauseCode : uint64 {
	SoftwareTimer = (1ul << 63) | 1,
	Hardware = (1ul << 63) | 9,

	IllegalInstruction = 2,
	LoadAccessFault = 5,
	StoreAccessFault = 7,
	UserSyscall = 8,
	SuperSyscall = 9
};

enum class SyscallCode : uint64 {
	MemoryAllocate = 0x01,
	MemoryFree = 0x02,
};

enum class SStatusBitmask : uint64 {
	SIE = (1 << 1),
	SPIE = (1 << 5),
	SPP = (1 << 8),
};

REGISTER_ACCESSOR(A0, a0)
REGISTER_ACCESSOR(A1, a1)
REGISTER_ACCESSOR(A2, a2)
REGISTER_ACCESSOR(A3, a3)
REGISTER_ACCESSOR(A4, a4)
REGISTER_ACCESSOR(A5, a5)
REGISTER_ACCESSOR(A6, a6)
REGISTER_ACCESSOR(A7, a7)

STATUS_ACCESSOR(STVec, stvec, uint64, uint64)
STATUS_ACCESSOR(SStatus, sstatus, uint64, SStatusBitmask)
STATUS_ACCESSOR(SEPC, sepc, uint64, uint64)
STATUS_ACCESSOR(SCause, scause, SCauseCode, uint64)

inline void ecall() { __asm__ volatile("ecall"); }

inline void init() {
	Sys::STVec::write((uint64)&supervisorTrap);
	Sys::SStatus::set(Sys::SStatusBitmask::SIE);
}

inline void handleSyscall() {
	Sys::SyscallCode code = (Sys::SyscallCode)Sys::A0::read();
	uint64 ret = 0;
	uint64 args[] = {Sys::A1::read()};

	switch (code) {
	case Sys::SyscallCode::MemoryAllocate:
		ret = MemoryAllocator::getInstance().allocate(args[0]);
		break;
	case Sys::SyscallCode::MemoryFree:
		ret = MemoryAllocator::getInstance().free(args[0]);
		break;
	}

	Sys::A0::write(ret);
}

} // namespace Kernel::Sys