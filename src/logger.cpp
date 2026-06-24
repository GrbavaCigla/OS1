#include "../h/logger.hpp"
#include "../h/console.hpp"
#include "../h/helper.hpp"

namespace kernel {

void Logger::printFreeChunks() {
	console::print("FreeChunkNode list:\n");
	MemoryAllocator& alloc = MemoryAllocator::getInstance();
	for (MemoryAllocator::FreeChunkNode* node = alloc.head; node;
		 node = node->next) {
		console::print("  node 0x");
		console::print((size_t)node, 16);
		console::print(" blocks=");
		console::print(node->blocks);
		console::print(" prev=0x");
		console::print((size_t)node->prev, 16);
		console::print(" next=0x");
		console::print((size_t)node->next, 16);
		console::print("\n");
	}
}

void Logger::printAllocationHeaders() {
	console::print("AllocationHeader list:\n");
	MemoryAllocator& alloc = MemoryAllocator::getInstance();

	size_t start = helper::alignUp((size_t)HEAP_START_ADDR, MEM_BLOCK_SIZE);
	size_t end = (size_t)HEAP_END_ADDR;

	for (size_t addr = start; addr < end;) {
		bool free = false;
		for (MemoryAllocator::FreeChunkNode* node = alloc.head; node;
			 node = node->next) {
			if ((size_t)node == addr) {
				free = true;
				addr += node->blocks * MEM_BLOCK_SIZE;
				break;
			}
		}
		if (free)
			continue;

		MemoryAllocator::AllocationHeader* header =
			(MemoryAllocator::AllocationHeader*)addr;
		console::print("  header 0x");
		console::print(addr, 16);
		console::print(" blocks=");
		console::print(header->blocks);
		console::print(" next=0x");
		console::print(header->next, 16);
		console::print(" flag=");
		console::print(header->flag);
		console::print("\n");
		addr += header->blocks * MEM_BLOCK_SIZE;
	}
}

void Logger::printThread(Thread* thread) {
	console::print("  thread 0x");
	console::print((size_t)thread, 16);
	console::print(" finished=");
	console::print((unsigned)thread->finished);
	if (thread == Thread::running)
		console::print(" (running)");
	console::print("\n");
}

void Logger::printReadyThreads() {
	console::print("Ready threads:\n");
	Scheduler<RoundRobin>& sched = Scheduler<RoundRobin>::getInstance();
	Scheduler<RoundRobin>::ThreadNode* ring = sched.readyQueue;
	if (!ring)
		return;
	Scheduler<RoundRobin>::ThreadNode* node = ring;
	do {
		printThread(node->thread);
		node = node->next;
	} while (node != ring);
}

void Logger::printSleepThreads() {
	console::print("Sleeping threads:\n");
	Scheduler<RoundRobin>& sched = Scheduler<RoundRobin>::getInstance();
	for (Scheduler<RoundRobin>::ThreadNode* node = sched.sleepQueue; node;
		 node = node->next) {
		console::print("  thread 0x");
		console::print((size_t)node->thread, 16);
		console::print(" finished=");
		console::print((unsigned)node->thread->finished);
		console::print(" ticks=");
		console::print(node->ticks);
		console::print("\n");
	}
}

void Logger::printScheduler() {
	printReadyThreads();
	printSleepThreads();
}

void Logger::printSemaphore(Semaphore* sem) {
	console::print("Semaphore 0x");
	console::print((size_t)sem, 16);
	console::print(" value=");
	if (sem->value < 0) {
		console::print("-");
		console::print((uint64)(-sem->value));
	} else {
		console::print((uint64)sem->value);
	}
	console::print(" closed=");
	console::print((unsigned)sem->closed);
	console::print("\n");

	console::print("  blocked threads:\n");
	Scheduler<RoundRobin>::ThreadNode* ring = sem->threads;
	if (!ring)
		return;
	Scheduler<RoundRobin>::ThreadNode* node = ring;
	do {
		console::print("    thread 0x");
		console::print((size_t)node->thread, 16);
		console::print("\n");
		node = node->next;
	} while (node != ring);
}

void Logger::printSemaphores() {
	console::print("Semaphore list:\n");
	for (Semaphore* sem = Semaphore::head; sem; sem = sem->next)
		printSemaphore(sem);
}

} // namespace kernel
