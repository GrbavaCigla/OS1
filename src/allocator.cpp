#include "../h/allocator.hpp"
#include "../h/helper.hpp"

namespace Kernel {

MemoryAllocator::MemoryAllocator() {
	size_t start = Helper::align_up((size_t)HEAP_START_ADDR, MEM_BLOCK_SIZE);
	size_t end = (size_t)HEAP_END_ADDR;

	this->head = (FreeChunkNode*)start;
	this->head->blocks = (end - start) / MEM_BLOCK_SIZE;
	this->head->next = nullptr;
	this->head->prev = nullptr;
}

size_t MemoryAllocator::allocate(size_t blocks) {
	blocks++;

	FreeChunkNode* ff = this->head;
	while (ff && ff->blocks < blocks)
		ff = ff->next;

	if (!ff)
		return 0;

	ff->blocks -= blocks;
	size_t start = next_chunk(ff);

	if (ff->blocks == 0) {
		if (ff->prev)
			ff->prev->next = ff->next;
		if (ff->next)
			ff->next->prev = ff->prev;
		if (this->head == ff)
			this->head = ff->next;
	}

	AllocationHeader* ah = (AllocationHeader*)start;
	ah->blocks = blocks;
	ah->next = start + MEM_BLOCK_SIZE;

	return start + MEM_BLOCK_SIZE;
}

int MemoryAllocator::free(size_t ptr) {
	if (!ptr)
		return -1;

	AllocationHeader* ah = (AllocationHeader*)(ptr - MEM_BLOCK_SIZE);
	if (ah->next != ptr)
		return -2;
	FreeChunkNode* newNode = (FreeChunkNode*)ah;
	newNode->blocks = ah->blocks;

	FreeChunkNode* cur = this->head;
	FreeChunkNode* prev = nullptr;
	while (cur && cur < newNode) {
		prev = cur;
		cur = cur->next;
	}

	newNode->next = cur;
	newNode->prev = prev;

	if (prev)
		prev->next = newNode;
	else
		this->head = newNode;
	if (cur)
		cur->prev = newNode;

	this->join(newNode);
	this->join(newNode->prev);
	return 0;
}

void MemoryAllocator::join(FreeChunkNode* node) {
	if (!node)
		return;

	size_t next = next_chunk(node);
	if (node->next && (FreeChunkNode*)next == node->next) {
		node->blocks += node->next->blocks;
		node->next = node->next->next;
		if (node->next)
			node->next->prev = node;
	}
}

} // namespace Kernel
