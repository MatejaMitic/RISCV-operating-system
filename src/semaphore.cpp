#include "../h/semaphore.hpp"
#include "../h/syscall_c.hpp"


_sem* _sem::createSemaphore(unsigned init)
{
    return new _sem(init);
}

int _sem::wait()
{
    if(!active){ return -1; }

    if(--(this->value) < 0)
    {
        waitingList.addLast(TCB::running);
        TCB::running->setBlocked(true);

        thread_dispatch();

        if(!active) { return -2; }
    }
    return 0;
}

int _sem::signal()
{
    if(!active) { return -1; }

    if(++(this->value) <= 0)
    {
        TCB* unblocked = waitingList.removeFirst();
        unblocked->setBlocked(false);
        Scheduler::put(unblocked);
    }
    return 0;
}

int _sem::close()
{
    if(!active) { return -1; }

    active = false;

    while(waitingList.peekFirst() != nullptr)
    {
        TCB* unblocked = waitingList.removeFirst();
        unblocked->setBlocked(false);
        Scheduler::put(unblocked);
    }
    return 0;
}

int _sem::tryWait()
{
    if(value <= 0) { return 1; }
    value--;
    return 0;

}


