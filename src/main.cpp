#include "../h/helper.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys.hpp"
#include "../h/thread.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"

void func1() {
	volatile int a = 10;
	volatile int b = 20;
	volatile int c = a + b;
	volatile int d = a - c - b;
	(void)a;
	(void)b;
	(void)c;
	(void)d;
	kernel::helper::print("kita1");
}

void func2() {
	volatile int a = 100;
	volatile int b = 200;
	volatile int c = a + b;
	volatile int d = a - c - b;
	(void)a;
	(void)b;
	(void)c;
	(void)d;
	kernel::helper::print("kita2");
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