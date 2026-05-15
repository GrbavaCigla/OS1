#include "../h/sys.hpp"
#include "../h/helper.hpp"
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
		helper::print("other");
		break;
	case SCauseCode::UserSyscall:
	case SCauseCode::SuperSyscall:
		helper::print("syscall");
		SEPC::write(SEPC::read() + 4);
		handleSyscall();
		break;
	}

	console_handler();
}

extern "C" void supervisorTrap();
} // namespace kernel::sys
