#pragma once

#include "../lib/hw.h"
#include "buffer.hpp"

namespace kernel::console {

extern InputBuffer* inputBuffer;
extern OutputBuffer* outputBuffer;

inline void flushOutput() {
	char data = *(char*)CONSOLE_STATUS;
	while ((data & (char)CONSOLE_TX_STATUS_BIT) && !outputBuffer->isEmpty()) {
		*(uint64*)CONSOLE_TX_DATA = outputBuffer->get();
		data = *(char*)CONSOLE_STATUS;
	}
}

inline void bufferInput() {
	char data = *(char*)CONSOLE_STATUS;
	while (data & CONSOLE_RX_STATUS_BIT) {
		char c = *(char*)CONSOLE_RX_DATA;
		if (!inputBuffer->isFull())
			inputBuffer->put(c);
		data = *(char*)CONSOLE_STATUS;
	}
}

void print(const char* s);

template<typename T>
void print(T x) {
	uint64 b = 0;
	int count = 0;
	do {
		b *= 10;
		b += x % 10;
		x /= 10;
		count++;
	} while (x != 0);
	for (int i = 0; i < count; i++) {
		outputBuffer->put(b % 10 + '0');
		b /= 10;
	}
}

void init();
void start();
void stop();

} // namespace kernel::console
