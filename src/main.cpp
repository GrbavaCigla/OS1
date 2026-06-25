#include "../h/console.hpp"
#include "../h/logger.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../test/userMain.hpp"

static volatile bool isUserThreadFinished = false;

void user(void*) {
	userMain();
	isUserThreadFinished = true;
}

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler::getInstance().add(&kernelThread);

	kernel::sys::exitSupervisor();

	kernel::console::print("=== memory before userMain ===\n");
	kernel::Logger::printFreeChunks();
	kernel::Logger::printAllocationHeaders();

	kernel::console::start();

	thread_t userThread;
	thread_create(&userThread, user, nullptr);

	while (!isUserThreadFinished)
		thread_dispatch();

	kernel::console::stop();

	while (kernel::Scheduler::getInstance().hasPending())
		thread_dispatch();

	kernel::Scheduler::getInstance().cleanup();
	kernel::Semaphore::cleanup();
	kernel::console::print("=== memory after userMain ===\n");
	kernel::Logger::printFreeChunks();
	kernel::Logger::printAllocationHeaders();

	kernel::console::flushOutput();
	kernel::sys::exit(kernel::sys::ExitStatus::Pass);

	return 0;
}
