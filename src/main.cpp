#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/semaphore.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../test/userMain.hpp"

// static sem_t s;
// static sem_t done;

// static void waiter(void*) {
//     kernel::helper::print("waiter: blocking\n");
//     int b = sem_wait(s);
// 	kernel::helper::print(b);
//     kernel::helper::print("\nwaiter: unblocked\n");
//     sem_signal(done);
//     kernel::helper::print("waiter: signal done\n");
// }

// static void signaler(void*) {
//     kernel::helper::print("--- before signal ---\n");
//     kernel::Semaphore::print();
//     sem_signal(s);
//     kernel::helper::print("--- after signal ---\n");
//     kernel::Semaphore::print();
//     sem_signal(done);
//     kernel::helper::print("signaler: signal done\n");
// }

int main() {
	kernel::sys::init();

	kernel::Thread kernelThread = kernel::Thread();
	kernel::Thread::running = &kernelThread;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&kernelThread);

	kernel::sys::exitSupervisor();

	userMain();

	// sem_open(&s, 0);
	// sem_open(&done, 0);

	// thread_t t1, t2;
	// thread_create(&t1, waiter, nullptr);
	// thread_create(&t2, signaler, nullptr);

	// thread_dispatch();

	// sem_wait(done);
	// sem_wait(done);

	// kernel::helper::print("--- final state ---\n");
	// kernel::Semaphore::print();

	// kernel::helper::print("--- end ---\n");

	while (true) {
	
	}

	return 0;
}
