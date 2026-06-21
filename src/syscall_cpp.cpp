#include "../h/syscall_cpp.hpp"

void* operator new(size_t size) { return mem_alloc(size); }
void operator delete(void* ptr) noexcept { mem_free(ptr); }

Thread::Thread(void (*body)(void*), void* arg)
    : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread()
    : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {
    return 0;
}
