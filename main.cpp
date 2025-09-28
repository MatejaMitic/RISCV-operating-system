//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"

extern void userMain();

void mainWrapper(void* arg)
{
    userMain();
}

int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv:: ms_sstatus(Riscv::SSTATUS_SIE);

    thread_t threads[5];

    thread_create(&threads[0], nullptr, nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], &mainWrapper, nullptr);

    while(!threads[1]->isFinished())
    {
        thread_dispatch();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }
    printString("Finished\n");
    return 0;
}
