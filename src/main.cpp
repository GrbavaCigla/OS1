#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"

void func1() {
    while (true) {
        kernel::helper::print("1");
        for (volatile int i = 0; i < 10000000; i++);
    }
}

void func2() {
    while (true) {
        kernel::helper::print("2");
        for (volatile int i = 0; i < 10000000; i++);
    }
}


int main() {
	kernel::sys::init();
	kernel::Scheduler& scheduler = kernel::Scheduler::getInstance();

	kernel::Thread kernel = kernel::Thread();
	scheduler.add(&kernel);

	kernel::Thread user2 = kernel::Thread(func2);
	scheduler.add(&user2);

	kernel::Thread user1 = kernel::Thread(func1);
	scheduler.add(&user1);

	kernel::Thread::dispatch();

	return 0;
}