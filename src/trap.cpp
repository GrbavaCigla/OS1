#include "../h/trap.hpp"
#include "../h/helper.hpp"
#include "../lib/console.h"

extern "C" void handleSupervisorTrap() {
	uint64 scause;
	__asm__ volatile("csrr %0, scause" : "=r"(scause));
	if (static_cast<SCause>(scause) != SCause::SoftwareTimer) {
		printInt(scause >> 63);
		__putc(' ');
		printInt(scause & ~(1ul << 63));
		__putc('\n');
	}

    console_handler();
}

extern "C" void supervisorTrap();

void registerSupervisorTrap() {
	__asm__ volatile("csrw stvec, %0" : : "r"(&supervisorTrap));
	__asm__ volatile("csrs sstatus, 0x02");
}