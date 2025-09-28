#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include"tcb.hpp"
#include "list.hpp"
#include "systemPrint.hpp"

class _sem {

public:
    static _sem* createSemaphore(unsigned init);

    int wait();

    int signal();

    int close();

    int tryWait();

private:
    _sem(unsigned init = 1) : value(init), active(true){}

    int value;
    bool active;
    List<TCB> waitingList;
};



#endif
