#pragma once

#include "scheduler.hpp"
#include "semaphore.hpp"
#include "thread.hpp"

namespace kernel {

class Logger {
  public:
	Logger() = delete;

	static void printFreeChunks();
	static void printAllocationHeaders();

	static void printReadyThreads();
	static void printSleepThreads();
	static void printScheduler();

	static void printSemaphore(Semaphore* sem);
	static void printSemaphores();

  private:
	static void printThread(Thread* thread);
};

} // namespace kernel
