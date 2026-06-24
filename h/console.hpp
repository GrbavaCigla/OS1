#pragma once

#include "../lib/hw.h"
#include "buffer.hpp"

namespace kernel::console {

static volatile bool finished = false;

extern Buffer<>* inputBuffer;
extern Buffer<>* outputBuffer;

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
void print(char c);

template <typename T> void print(T x, unsigned base = 10) {
	static const char digits[] = "0123456789abcdef";

	uint64 b = 0;
	int count = 0;
	do {
		b *= base;
		b += x % base;
		x /= base;
		count++;
	} while (x != 0);
	for (int i = 0; i < count; i++) {
		if (outputBuffer->isFull())
			flushOutput();
		outputBuffer->put(digits[b % base]);
		b /= base;
	}
}

void thread(void*);
void init();
void start();
void stop();

} // namespace kernel::console
