#include "../h/console.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../test/userMain.hpp"

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	kernel::sys::exitSupervisor();
	kernel::console::start();

	userMain();
	
	kernel::console::stop();
	kernel::sys::exit(kernel::sys::ExitStatus::Pass);

	return 0;
}
