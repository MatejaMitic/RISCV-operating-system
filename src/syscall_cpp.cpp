#include "../h/syscall_cpp.hpp"

Thread::Thread(void (*body)(void*), void* arg)
{
    this->body = body;
    this->arg = arg;
}

Thread::Thread()
{
//    this->body=Thread::wrapper;
//    this->arg = this;
    arg = this;
    body = [](void* arg){
        auto thread = (Thread *) arg;
        thread->run();
    };
}

Thread::~Thread()
{
    delete myHandle;
}

int Thread::start()
{
    return thread_create(&myHandle, this->body, this->arg);
}

void Thread::dispatch()
{
    thread_dispatch();
}

int Thread::sleep(time_t time)
{
    return 0;
}

void Thread::wrapper(void *threadArg)
{
    if(threadArg)
    {
        auto myThread = (Thread*) threadArg;
        myThread->run();
    }
}

Semaphore::Semaphore(unsigned init)
{
    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore()
{
    sem_close(this->myHandle);
}

int Semaphore::signal()
{
    return sem_signal(this->myHandle);
}

int Semaphore::wait()
{
    return sem_wait(this->myHandle);
}

int Semaphore::timedWait(time_t)
{
    return 0;
}

int Semaphore::tryWait()
{
    return sem_trywait(this->myHandle);
}

char Console::getc()
{
    return syscall_c::getc();
}

void Console::putc(char c)
{
    return syscall_c::putc(c);
}
