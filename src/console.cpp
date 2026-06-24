#include "../h/console.hpp"

namespace kernel::console {

Buffer<>* inputBuffer = nullptr;
Buffer<>* outputBuffer = nullptr;

void print(const char* s) {
	int i = 0;
	while (*(s + i) != '\0') {
		if (outputBuffer->isFull())
			flushOutput();
		outputBuffer->put(*(s + i));
		i++;
	}
}

void print(char c) {
	if (outputBuffer->isFull())
		flushOutput();
	outputBuffer->put(c);
}

void init() {
	inputBuffer = new Buffer<>();
	outputBuffer = new Buffer<>();
}

void thread(void*) {
	while (!finished) {
		flushOutput();
		thread_dispatch();
	}
}

void start() {
	thread_t t;
	thread_create(&t, thread, nullptr);
}

void stop() {
	finished = true;
	flushOutput();
}

} // namespace kernel::console
