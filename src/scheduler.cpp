#include "../h/scheduler.hpp"
#include "../h/allocator.hpp"
#include "../h/helper.hpp"
#include "../h/sys.hpp"

namespace kernel {

Thread* Scheduler::current() {
	if (!head)
		return nullptr;
	return head->thread;
}

void Scheduler::next() {
	if (!head)
		return;
	head = head->next;
}

void Scheduler::add(Thread* thread) {
	ThreadNode* node = (ThreadNode*)MemoryAllocator::getInstance().allocate(
		helper::roundUp(sizeof(ThreadNode)));
	node->thread = thread;

	if (!head) {
		node->next = node;
		node->prev = node;
		head = node;
		return;
	}

	ThreadNode* tail = head->prev;
	tail->next = node;
	node->prev = tail;
	node->next = head;
	head->prev = node;
}

} // namespace kernel