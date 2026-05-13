#include "../h/sys.hpp"
#include "../h/helper.hpp"
#include "../lib/console.h"

namespace Kernel::Sys {
extern "C" void handleSupervisorTrap() {
	SCauseCode scause = SCause::read();

	switch (scause) {
	case SCauseCode::SoftwareTimer:
		break;
	case SCauseCode::Hardware:
	case SCauseCode::IllegalInstruction:
	case SCauseCode::LoadAccessFault:
	case SCauseCode::StoreAccessFault:
		Helper::print("other");
		break;
	case SCauseCode::UserSyscall:
	case SCauseCode::SuperSyscall:
		Helper::print("syscall");
		SEPC::write(SEPC::read() + 4);
		handleSyscall();
		break;
	}

	console_handler();
}

extern "C" void supervisorTrap();
} // namespace Kernel::Sys
