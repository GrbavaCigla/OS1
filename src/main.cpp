#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/semaphore.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../lib/hw.h"
#include "../test/userMain.hpp"

static volatile bool gotChar = false;

static void charReader(void*) {
	getc();
	gotChar = true;
}

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	kernel::sys::exitSupervisor();

	thread_t reader;
	thread_create(&reader, charReader, nullptr);
	thread_dispatch();

	while (!gotChar)
		thread_dispatch();

	kernel::sys::exit(kernel::sys::ExitStatus::Pass);

	return 0;
}
