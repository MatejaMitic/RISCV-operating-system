#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct DataBlock
{
    size_t size;
    DataBlock* next;
    DataBlock* previous;
};

class memoryAllocator
{
public:
    static memoryAllocator& create()
    {
        static  memoryAllocator instance;
        return instance;
    }

    static DataBlock *head;

    static void* malloc(size_t size);


    static int free(void* ptr);

    size_t mem_get_free_space();

    size_t mem_get_largest_free_block();

    static void mergeFree(DataBlock* left, DataBlock* right);

    static void initializeFreeBlock();

private:
    static DataBlock* freeList;
    memoryAllocator(){};
    //memoryAllocator(const memoryAllocator&) = delete;
    //MemoryAlocator& operator=(const memoryAllocator&) = delete;
    //static Block *head;
    static void compact(DataBlock* currentBlock);



};


#endif
