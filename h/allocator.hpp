#pragma once

#include "../lib/hw.h"

namespace kernel {

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
		size_t next;
	};

	FreeChunkNode* head;

	void join(FreeChunkNode*);

	static inline size_t nextChunk(FreeChunkNode* node) {
		return reinterpret_cast<size_t>(node) + (node->blocks * MEM_BLOCK_SIZE);
	}
};

} // namespace kernel
