#include "../h/memoryAllocator.hpp"

DataBlock* memoryAllocator::freeList = nullptr;

void memoryAllocator::initializeFreeBlock() {
    freeList       = (DataBlock*) HEAP_START_ADDR;
    freeList->size = (size_t)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR);
    freeList->next = nullptr;
}

static inline size_t align_up(size_t n) {
    size_t r = n % MEM_BLOCK_SIZE;
    return r ? (n + (MEM_BLOCK_SIZE - r)) : n;
}

void* memoryAllocator::malloc(size_t requestedSize) {
    if (!freeList) initializeFreeBlock();

    size_t need = align_up(requestedSize + sizeof(DataBlock));

    DataBlock* prev = nullptr;
    DataBlock* curr = freeList;
    while (curr && curr->size < need) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return nullptr;

    if (curr->size < need + MEM_BLOCK_SIZE) {
        if (prev) prev->next = curr->next;
        else freeList = curr->next;
        curr->next = nullptr;
        return (void*)(curr + 1);
    }

    DataBlock* remaining = (DataBlock*)((char*)curr + need);
    remaining->size = curr->size - need;
    remaining->next = curr->next;
    if (prev) prev->next = remaining;
    else freeList = remaining;
    curr->size = need;
    curr->next = nullptr;
    return (void*)(curr + 1);
}

int memoryAllocator::free(void* head) {
    if (!head) return -1;

    DataBlock* node = (DataBlock*)head - 1;
    DataBlock* prev = nullptr;
    DataBlock* nxt  = freeList;

    while (nxt && node > nxt) {
        prev = nxt;
        nxt  = nxt->next;
    }
    if (nxt == node) return -2;

    node->next = nxt;
    if (prev) prev->next = node;
    else freeList = node;

    mergeFree(prev, node);
    mergeFree(node, nxt);

    return 0;
}

void memoryAllocator::mergeFree(DataBlock* left, DataBlock* right) {
    if (!left || !right) return;
    if ((char*)left + left->size == (char*)right) {
        left->size += right->size;
        left->next = right->next;
        right->next = nullptr;
    }
}

size_t memoryAllocator::mem_get_free_space() {
    size_t sum = 0;
    for (DataBlock* p = freeList; p; p = p->next) sum += p->size;
    return sum;
}

size_t memoryAllocator::mem_get_largest_free_block() {
    size_t best = 0;
    for (DataBlock* p = freeList; p; p = p->next)
        if (p->size > best) best = p->size;
    return best;
}