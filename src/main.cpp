#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/semaphore.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../test/userMain.hpp"

// static void sleeper(void* arg) {
// 	char id = (char)(uint64)arg;
// 	uint64 period = (id - '0') * 30;
// 	for (int i = 0; i < 3; i++) {
// 		time_sleep(period);
// 		kernel::helper::print("woke ");
// 		putc(id);
// 		putc('\n');
// 	}
// }

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	kernel::sys::exitSupervisor();

	// userMain();

	// thread_t t1, t2, t3;
	// thread_create(&t1, sleeper, (void*)'1');
	// thread_create(&t2, sleeper, (void*)'2');
	// thread_create(&t3, sleeper, (void*)'3');

	kernel::sys::exit(kernel::sys::ExitStatus::Pass);
	// while (true) {
	// 	thread_dispatch();
	// }

	

	return 0;
}
