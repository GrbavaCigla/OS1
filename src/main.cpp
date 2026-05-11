#include "../lib/console.h"
#include "../lib/hw.h"
#include "../h/trap.hpp"

int main() {
    registerSupervisorTrap();
    
    while (true) {
        
    }

    return 0;
}