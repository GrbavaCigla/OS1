#pragma once

#include "../lib/hw.h"

#define NEXT_CHK(node) (((size_t)(node)) + (node)->blocks * MEM_BLOCK_SIZE)

class MemoryAllocator {
  public:
	static MemoryAllocator& getInstance() {
		static MemoryAllocator instance;
		return instance;
	}

	MemoryAllocator(const MemoryAllocator&) = delete;
	MemoryAllocator& operator=(const MemoryAllocator&) = delete;

	size_t allocate(size_t);
	int free(size_t);

  private:
	MemoryAllocator();
	~MemoryAllocator() = default;

	struct FreeChunkNode {
		size_t blocks;
		FreeChunkNode* prev;
		FreeChunkNode* next;
	};

	struct AllocationHeader {
		size_t blocks;
	};

	FreeChunkNode* head;

	void join(FreeChunkNode*);
};
