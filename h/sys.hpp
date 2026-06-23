#pragma once

#include "../lib/hw.h"
#include "buffer.hpp"
#include "thread.hpp"

#define REGISTER_ACCESSOR(name, reg)                                           \
	struct name {                                                              \
		static inline uint64 read() {                                          \
			uint64 val;                                                        \
			__asm__ volatile("mv %0, " #reg : "=r"(val));                      \
			return val;                                                        \
		}                                                                      \
		static inline void write(uint64 val) {                                 \
			__asm__ volatile("mv " #reg ", %0" : : "r"(val) : #reg);           \
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

#define INSTRUCTION(name)                                                      \
	inline void name() { __asm__ volatile(#name); }

extern "C" void supervisorTrap();

extern "C" void handleSupervisorTrap();

namespace kernel::sys {

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
	ThreadCreate = 0x11,
	ThreadExit = 0x12,
	ThreadDispatch = 0x13,
	ThreadSleep = 0x31,
	SemaphoreOpen = 0x21,
	SemaphoreClose = 0x22,
	SemaphoreWait = 0x23,
	SemaphoreSignal = 0x24,
	SemaphoreWaitN = 0x25,
	SemaphoreSignalN = 0x26,
	ConsoleGetChar = 0x41,
	ConsolePutChar = 0x42,
};

enum class ExitStatus : uint32 {
	Pass = 0x5555,
	Fail = 0x3333,
	Reset = 0x7777,
};

enum class SStatusBitmask : uint64 {
	SIE = (1 << 1),
	SPIE = (1 << 5),
	SPP = (1 << 8),
};

enum class SIPBitmask : uint64 {
	SSIP = (1 << 1),
};

REGISTER_ACCESSOR(A0, a0)
REGISTER_ACCESSOR(A1, a1)
REGISTER_ACCESSOR(A2, a2)
REGISTER_ACCESSOR(A3, a3)
REGISTER_ACCESSOR(A4, a4)

STATUS_ACCESSOR(STVec, stvec, uint64, uint64)
STATUS_ACCESSOR(SStatus, sstatus, uint64, SStatusBitmask)
STATUS_ACCESSOR(SIP, sip, uint64, SIPBitmask)
STATUS_ACCESSOR(SEPC, sepc, uint64, uint64)
STATUS_ACCESSOR(SCause, scause, SCauseCode, uint64)

INSTRUCTION(ecall)

extern Buffer<BufferType::Input>* inputBuffer;
extern Buffer<BufferType::Output>* outputBuffer;

inline void init() {
	sys::STVec::write((uint64)&supervisorTrap);
	sys::SStatus::set(sys::SStatusBitmask::SIE);
	inputBuffer = new Buffer<BufferType::Input>();
	outputBuffer = new Buffer<BufferType::Output>();
}

inline void exit(ExitStatus status) {
	*(volatile uint32*)0x100000 = (uint32)status;
}

void exitSupervisor();

void contextSwitch(Thread::Context*, Thread::Context*);

} // namespace kernel::sys
