#include "../h/sys.hpp"
#include "../h/trap.hpp"

namespace kernel::sys {
extern "C" void handleSupervisorTrap() {
	uint64 code = A0::read();
	uint64 args[] = {A1::read(), A2::read(), A3::read(), A4::read()};

	SCauseCode scause = SCause::read();
	volatile uint64 sepc = SEPC::read();
	volatile uint64 sstatus = SStatus::read();

	uint64 ret = 0;

	switch (scause) {
	case SCauseCode::SoftwareTimer:
		SIP::clear(SIPBitmask::SSIP);
		handleTimer();
		break;
	case SCauseCode::Hardware:
		handleHardware();
		break;
	case SCauseCode::IllegalInstruction:
	case SCauseCode::LoadAccessFault:
	case SCauseCode::StoreAccessFault:
		if (!(sstatus & (uint64)SStatusBitmask::SPP)) {
			console::print("Thread killed");
			Thread::running->finished = true;
			Thread::dispatch();
		} else {
			console::print("Kernel panic :(");
			sys::exit(ExitStatus::Fail);
		}
		break;
	case SCauseCode::UserSyscall:
	case SCauseCode::SuperSyscall:
		sepc += 4;
		ret = handleSyscall(code, args);
		break;
	}

	SEPC::write(sepc);
	SStatus::write(sstatus);
	A0::write(ret);
}

extern "C" void supervisorTrap();

void exitSupervisor() {
	SStatus::clear(SStatusBitmask::SPP);
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
