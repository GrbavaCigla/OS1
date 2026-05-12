#include "../h/trap.hpp"
#include "../h/helper.hpp"
#include "../lib/console.h"

extern "C" void handleSupervisorTrap() {
	SCause scause;
	__asm__ volatile("csrr %0, scause" : "=r"(scause));
	switch (scause) {
	case SCause::SoftwareTimer:
	case SCause::Hardware:
	case SCause::IllegalInstruction:
	case SCause::LoadAccessFault:
	case SCause::StoreAccessFault:
		break;
	case SCause::UserSyscall:
	case SCause::SuperSyscall:
		handleSyscall();
		break;
	}

	console_handler();
}

extern "C" void supervisorTrap();