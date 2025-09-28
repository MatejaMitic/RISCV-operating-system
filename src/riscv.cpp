//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"

#include "../test/printing.hpp"
#include "../h/tcb.hpp"
#include "../h/semaphore.hpp"
#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../lib/mem.h"
#include "../h/systemPrint.hpp"
#include "../h/memoryAllocator.hpp"

using Body = void (*)(void*);

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 operationCode = Riscv::r_a0();

        sem_t semaphoreHandle;
        int returnValue;

        switch (operationCode)
        {
        //*MEMORY_ALLOCATOR*
        //mem_alloc()
        case 0x01:
            uint64 blocksNum;
            __asm__ volatile ("mv %0, a1" : "=r" (blocksNum));
            size_t bytes;
            bytes = blocksNum * MEM_BLOCK_SIZE;

            void* ptr;
            ptr = memoryAllocator::malloc(bytes);
            __asm__ volatile ("mv t0, %0" : : "r"(ptr));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //mem_free
        case 0x02:
            void* pointer;
            __asm__ volatile ("mv %0, a1" : "=r" (pointer));

            int returnVal;
            returnVal = memoryAllocator::free(pointer);

            __asm__ volatile ("mv t0, %0" : : "r"(returnVal));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //*THREAD*
        //thread_create()
        case 0x11:
            TCB** threadHandle;
            Body body;
            void* arg;
            uint64* stack;
            __asm__ volatile ("mv %0, a1" : "=r" (threadHandle));
            __asm__ volatile ("mv %0, a2" : "=r" (body));
            __asm__ volatile ("mv %0, a7" : "=r" (arg));
            stack = body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr;
            *threadHandle = TCB::createThread(body, arg, stack);
            if(*threadHandle != nullptr)
            {
                __asm__ volatile ("li t0, 0");
                __asm__ volatile ("sw t0, 80(x8)");
            } else
            {
                __asm__ volatile ("li t0, -1");
                __asm__ volatile ("sw t0, 80(x8)");
            }
            break;

        //thread_exit()
        case 0x12:
            TCB::running->setFinished(true);
            TCB::dispatch();

            __asm__ volatile ("li t0, 0");
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //thread_dispatch()
        case 0x13:
            TCB::dispatch();
            break;

        //*SEMAPHORE*
        //sem_open()
        case 0x21:
            sem_t* semHandle;
            unsigned init;

            __asm__ volatile ("mv %0, a2 " : "=r"(init));
            __asm__ volatile ("mv %0, a1 " : "=r"(semHandle));
            *semHandle =  _sem::createSemaphore(init);

            if(*semHandle != nullptr)
            {
                __asm__ volatile ("li t0, 0");
                __asm__ volatile ("sw t0, 80(x8)");
            }
            else
            {
                __asm__ volatile ("li t0, -1");
                __asm__ volatile ("sw t0, 80(x8)");
            }
            break;

        //sem_close()
        case 0x22:
            __asm__ volatile ("mv %0, a1 " : "=r"(semaphoreHandle));
            if(semaphoreHandle != nullptr)
            {
                returnValue = semaphoreHandle->close();
            }else { returnValue = -2; }
            __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //sem_wait()
        case 0x23:
            __asm__ volatile ("mv %0, a1 " : "=r"(semaphoreHandle));
            if(semaphoreHandle != nullptr)
            {
                returnValue = semaphoreHandle->wait();
            } else { returnValue = -2; }
            __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //sem_signal()
        case 0x24:
            __asm__ volatile ("mv %0, a1 " : "=r"(semaphoreHandle));
            if(semaphoreHandle != nullptr)
            {
                returnValue = semaphoreHandle->signal();
            } else { returnValue = -2; }
            __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //try_wait()
        case 0x26:
            __asm__ volatile ("mv %0, a1 " : "=r"(semaphoreHandle));
            if(semaphoreHandle != nullptr)
            {
                returnValue = semaphoreHandle->tryWait();
            } else { returnValue = -2; }
            __asm__ volatile ("mv t0, %0" : : "r"(returnValue));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //*CONSOLE*
        //getc()
        case 0x41:
            char returnChar;
            returnChar = __getc();
            __asm__ volatile ("mv t0, %0" : : "r"(returnChar));
            __asm__ volatile ("sw t0, 80(x8)");
            break;

        //putc()
        case 0x42:
            char ch;
            __asm__ volatile ("mv %0, a1 " : "=r"(ch));
            __putc(ch);
            break;
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        mc_sip(SIP_SSIP);
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else
    {
        // unexpected trap cause
        printString("ERROR - UNEXPECTED TRAP CAUSE\nSCAUSE:");
        printInt(scause);
        printString("\n");
        uint32 endCode = 0x5555;
        auto endAdr = (uint32*) 0x100000;
        *endAdr = endCode;
    }
}
