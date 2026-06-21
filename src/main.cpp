#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/syscall_c.hpp"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../test/userMain.hpp"

void func1(void* arg) {
    while (true) {
        kernel::helper::print("1");
        for (volatile int i = 0; i < 10000000; i++);
    }
}

void func2(void* arg) {
    while (true) {
        kernel::helper::print("2");
        for (volatile int i = 0; i < 10000000; i++);
    }
}

int main() {
	kernel::sys::init();

	kernel::Thread idle = kernel::Thread();
	kernel::Thread::running = &idle;
	kernel::Scheduler<kernel::RoundRobin>::getInstance().add(&idle);

	kernel::sys::exitSupervisor();

	userMain();

	// thread_t t1, t2;
	// thread_create(&t2, func2, nullptr);
	// thread_create(&t1, func1, nullptr);

	// thread_exit();

	return 0;
}
