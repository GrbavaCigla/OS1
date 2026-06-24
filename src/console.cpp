#include "../h/console.hpp"
#include "../h/syscall_c.hpp"

namespace kernel::console {

InputBuffer* inputBuffer = nullptr;
OutputBuffer* outputBuffer = nullptr;

static volatile bool finished = false;

static void consoleThread(void*) {
	while(!finished) {
		flushOutput();
		thread_dispatch();
	}
}

void print(const char* s) {
	int i = 0;
	while (*(s + i) != '\0') {
		outputBuffer->put(*(s + i));
		i++;
	}
}

void init() {
	inputBuffer = new InputBuffer();
	outputBuffer = new OutputBuffer();
}

void start() {
	thread_t t;
	thread_create(&t, consoleThread, nullptr);
}

void stop() {
	finished = true;
	flushOutput();
}

} // namespace kernel::console
