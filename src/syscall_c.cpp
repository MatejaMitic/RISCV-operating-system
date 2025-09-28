//
// Created by os on 8/15/24.
//

#include "../h/syscall_c.hpp"
#include "../h/systemPrint.hpp"

void* mem_alloc(size_t size)
{
    uint64 blocksNum = size / MEM_BLOCK_SIZE;
    if(size % MEM_BLOCK_SIZE != 0)
    {
        blocksNum = ((MEM_BLOCK_SIZE + size - 1) / MEM_BLOCK_SIZE);
    }
    __asm__ volatile("mv a1, %0" : : "r"(blocksNum));
    __asm__ volatile("li a0, 0x01");
    __asm__ volatile("ecall");

    void *startAdress;
    __asm__ volatile("mv %0, a0" : "=r"(startAdress));
    return startAdress;
}

int mem_free(void* ptr)
{
    __asm__ volatile("mv a1, %0" : : "r"(ptr));
    __asm__ volatile("li a0, 0x02");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg)
{
    __asm__ volatile("mv a7, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x11");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int thread_exit()
{
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

void thread_dispatch()
{
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init)
{
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int sem_close(sem_t handle)
{
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int sem_wait(sem_t id)
{
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}


int sem_signal(sem_t id)
{
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int sem_trywait(sem_t id)
{
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x26");
    __asm__ volatile("ecall");

    uint64 returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return (int) returnValue;
}

int time_sleep(time_t) { return  0; }

char getc()
{
    __asm__ volatile ("li a0, 0x41");
    __asm__ volatile ("ecall");

    char returnValue;
    __asm__ volatile("mv %0, a0" : "=r"(returnValue));
    return returnValue;
}

void putc(char c)
{
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile ("li a0, 0x42");
    __asm__ volatile ("ecall");
}
