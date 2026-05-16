#include "../h/sys.hpp"
#include "../h/helper.hpp"
#include "../h/trap.hpp"
#include "../lib/console.h"

namespace kernel::sys {
extern "C" void handleSupervisorTrap() {
	SCauseCode scause = SCause::read();

	switch (scause) {
	case SCauseCode::SoftwareTimer:
		break;
	case SCauseCode::Hardware:
	case SCauseCode::IllegalInstruction:
	case SCauseCode::LoadAccessFault:
	case SCauseCode::StoreAccessFault:
		// helper::print("other");
		break;
	case SCauseCode::UserSyscall:
	case SCauseCode::SuperSyscall:
		// helper::print("syscall");
		SEPC::write(SEPC::read() + 4);
		handleSyscall();
		break;
	}

	console_handler();
}

extern "C" void supervisorTrap();

void enterUserspace() {
	__asm__ volatile("csrw sepc, ra");
	__asm__ volatile("sret");
}

void contextSwitch(Thread::Context* oldContext, Thread::Context* newContext) {
	__asm__ volatile("sd ra, %0\n"
					 "sd sp, %1\n"
					 "ld ra, %2\n"
					 "ld sp, %3\n"
					 :
					 : "m"(oldContext->ra), "m"(newContext->sp),
					   "m"(oldContext->ra), "m"(newContext->sp));
}

} // namespace kernel::sys
