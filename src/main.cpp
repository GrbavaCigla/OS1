#include "../h/console.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../test/userMain.hpp"

static volatile bool isUserThreadFinished = false;

static void user(void*) {
	// // kernel::Logger& logger = kernel::Logger::getInstance();

	// // logger.printFreeMemory();

	// kernel::sys::exitSupervisor();
	// userMain();

	// // logger.printFreeMemory();
	putc('a');
	putc('b');
	putc('c');
	putc('d');
	putc('e');
	putc('f');
	putc('g');
	putc('h');
	putc('i');
	putc('j');
	putc('k');
	putc('l');
	putc('m');
	putc('n');
	putc('o');
	putc('\n');

	isUserThreadFinished = true;

}

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	kernel::Thread userThread = kernel::Thread(user, nullptr);
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&userThread);

	while (!isUserThreadFinished) {
		kernel::console::flushOutput();
		thread_dispatch();
	}
	kernel::console::flushOutput();
	kernel::sys::exit(kernel::sys::ExitStatus::Pass);

	return 0;
}
