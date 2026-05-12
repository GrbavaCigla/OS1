#pragma once

#define ALIGN_UP(x, size) (((x) + size - 1) & ~(size - 1))
#define ROUND_UP(n, d) (((n) + (d) - 1) / (d))

void printInt(unsigned long int);

void printStr(const char*);