#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../test/userMain.hpp"

static volatile bool userThreadFinished = false;

void user(void*) {
	userMain();
	userThreadFinished = true;
}

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	// kernel::sys::exitSupervisor();

	thread_t userThread;
	thread_create(&userThread, user, nullptr);

	while(true) {
		char data = *(char*)CONSOLE_STATUS;
		while ((data & (char)CONSOLE_TX_STATUS_BIT) &&
		       !kernel::sys::outputBuffer->isEmpty()) {
			*(uint64*)CONSOLE_TX_DATA = kernel::sys::outputBuffer->get();
			data = *(char*)CONSOLE_STATUS;
		}

		thread_dispatch();
		if (userThreadFinished) break;
	}

	kernel::sys::exit(kernel::sys::ExitStatus::Pass);

	return 0;
}
