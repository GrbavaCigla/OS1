#include "../h/sys.hpp"
#include "../h/trap.hpp"
#include "../lib/console.h"

namespace kernel::sys {
extern "C" void handleSupervisorTrap() {
	SCauseCode scause = SCause::read();
	volatile uint64 sepc = SEPC::read();
	volatile uint64 sstatus = SStatus::read();

	switch (scause) {
	case SCauseCode::SoftwareTimer:
		SIP::clear(SIPBitmask::SSIP);
		handleTimer();
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
		sepc += 4;
		handleSyscall();
		break;
	}

	SEPC::write(sepc);
	SStatus::write(sstatus);

	console_handler();
}

extern "C" void supervisorTrap();

void enterUserspace() {
	__asm__ volatile("csrw sepc, ra");
	__asm__ volatile("sret");
}

void __attribute__((naked)) contextSwitch(Thread::Context* oldContext,
										  Thread::Context* newContext) {
	__asm__ volatile("sd ra, %c0(a0)\n"
					 "sd sp, %c1(a0)\n"
					 "ld ra, %c0(a1)\n"
					 "ld sp, %c1(a1)\n"
					 "ret"
					 :
					 : "i"(__builtin_offsetof(Thread::Context, ra)),
					   "i"(__builtin_offsetof(Thread::Context, sp)));
}

} // namespace kernel::sys
