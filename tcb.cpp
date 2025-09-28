//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/systemPrint.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;

TCB *TCB::createThread(Body body, void* arg, void* stackSpace)
{
    return new TCB(body, arg, stackSpace);
}

void TCB::yield()
{
    thread_dispatch();
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}
