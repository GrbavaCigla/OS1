#include "../h/console.hpp"
#include "../h/syscall_c.hpp"

namespace kernel::console {

InputBuffer* inputBuffer = nullptr;
OutputBuffer* outputBuffer = nullptr;


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

} // namespace kernel::console
