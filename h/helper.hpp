#pragma once

#define ALIGN_UP(x, size) (((x) + size - 1) & ~(size - 1))

void printInt(unsigned long int);

void printStr(const char*);