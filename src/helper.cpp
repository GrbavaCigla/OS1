#include "../lib/console.h"

void printInt(unsigned long int x) {
	unsigned long int b = 0;
	do {
		b *= 10;
		b += x % 10;
		x /= 10;
	} while (x != 0);
	do {
		__putc(b % 10 + '0');
		b /= 10;
	} while (b != 0);
	__putc('\n');
}

void printStr(const char* s) {
    int i = 0;
	while (*(s + i) != '\0') {
		__putc(*(s + i));
		i++;
	}
	__putc('\n');
}