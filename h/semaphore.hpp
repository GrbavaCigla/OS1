#pragma once

#include "../lib/hw.h"

namespace kernel {

class Semaphore {
  public:
	Semaphore(unsigned init);

	int wait(unsigned n = 1);
	int signal(unsigned n = 1);
	void deallocate();

	int value;

  private:
	static Semaphore* head;
	Semaphore* next;
	Semaphore* prev;
};

} // namespace kernel
