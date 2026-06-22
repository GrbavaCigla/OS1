#include "../h/syscall_cpp.hpp"

void* operator new(size_t size) { return mem_alloc(size); }
void operator delete(void* ptr) noexcept { mem_free(ptr); }

Thread::Thread(void (*body)(void*), void* arg)
	: myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {}

void Thread::runWrapper(void* arg) {
	Thread* self = static_cast<Thread*>(arg);
	self->run();
}

int Thread::start() {
	if (body)
		return thread_create(&myHandle, body, arg);
	return thread_create(&myHandle, &Thread::runWrapper, this);
}

void Thread::dispatch() { thread_dispatch(); }

int Thread::sleep(time_t period) { return time_sleep(period); }

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
	sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
	sem_close(myHandle);
}

int Semaphore::wait() {
	return sem_wait(myHandle);
}

int Semaphore::signal() {
	return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period)
	: Thread(), period(period), terminated(false) {}

void PeriodicThread::terminate() { terminated = true; }

void PeriodicThread::run() {
	while (!terminated) {
		periodicActivation();
		sleep(period);
	}
}

char Console::getc() { return ::getc(); }

void Console::putc(char c) { ::putc(c); }
