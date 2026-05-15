#include "../h/helper.hpp"

#include "../lib/console.h"

namespace kernel::helper {

void print(const char* s) {
	int i = 0;
	while (*(s + i) != '\0') {
		__putc(*(s + i));
		i++;
	}
}

} // namespace kernel::helper
