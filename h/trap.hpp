#pragma once

#include "../lib/hw.h"

enum class SCause : uint64 {
	SoftwareTimer = (1ul << 63) | 1,
	Hardware = (1ul << 63) | 9,

	IllegalInstruction = 2,
	LoadAccessFault = 5,
	StoreAccessFault = 7,
	UserSyscall = 8,
	SuperSyscall = 9
};

extern "C" void handleSupervisorTrap();

extern "C" void supervisorTrap();

void registerSupervisorTrap();