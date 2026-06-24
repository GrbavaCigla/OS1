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

void init() {
	inputBuffer = new Buffer<>();
	outputBuffer = new Buffer<>();
}

} // namespace kernel::console
