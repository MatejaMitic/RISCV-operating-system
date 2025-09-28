# Table of Contents
- [Project Overview](#project-overview)  
- [Key Features](#key-features)  
- [Core Components and Technical Deep Dive](#core-components-and-technical-deep-dive)  
  1. [Memory Management](#1-memory-management)  
  2. [Thread Management](#2-thread-management)  
  3. [Semaphores and Synchronization](#3-semaphores-and-synchronization)  
  4. [Console I/O](#5-console-io)  
- [Example Usage](#example-usage)  
- [How to Compile and Run](#how-to-compile-and-run)  

---

## Project Overview
This project implements a **lightweight operating system kernel** for the RISC-V architecture, written in C++ with optional inline assembly.  
The kernel supports multithreading, preemptive context switching, memory allocation, semaphores for synchronization, and basic console I/O.  
It runs in the **QEMU RISC-V emulator**, making it suitable for embedded systems education and research.  

---

## Key Features
- **Thread Abstraction**: Creation, dispatching, and termination of user threads.  
- **Memory Allocator**: Continuous memory allocation and deallocation using a custom `MemoryAllocator`.  
- **Preemption**: Context switching triggered by timer interrupts.  
- **Semaphores**: Robust synchronization primitives with support for blocking, timed, and non-blocking wait.  
- **Console I/O**: Input and output using interrupts from a simulated UART console.  
- **C and C++ APIs**: Layered system call interface for both procedural and object-oriented usage.  

---

## Core Components and Technical Deep Dive

### 1. Memory Management
- Implemented via a custom **`MemoryAllocator`** class.  
- Provides aligned allocation in blocks (`MEM_BLOCK_SIZE`).  
- System calls: `mem_alloc`, `mem_free`.  

### 2. Thread Management
- Each thread has its own **context (registers, stack)**.  
- System calls: `thread_create`, `thread_exit`, `thread_dispatch`.  
- `Thread` C++ class wraps system calls for easier usage.  

### 3. Semaphores and Synchronization
- Supports `sem_open`, `sem_wait`, `sem_signal`, `sem_close`.  
- Prevents race conditions in shared data access.  
- Implements **blocking, try-wait, and timed-wait** semantics.  

### 4. Preemption
- Hardware timer generates periodic interrupts (~10 per second).  
- On each interrupt, the scheduler can perform context switching.  
- Provides responsiveness to external events and fairness among threads.  

### 5. Console I/O
- Console is accessed through **UART interrupts**.  
- `Console::getc()` suspends thread until input arrives.  
- `Console::putc(char)` outputs to terminal.  

---

## Example Usage
```cpp
#include "syscall_cpp.hpp"

void worker(void*) {
    for (int i = 0; i < 5; i++) {
        Console::putc('A' + i);
        Thread::sleep(10);
    }
}

int main() {
    Thread t(worker, nullptr);
    t.start();
    return 0;
}
```
## How to Compile and Run

### Prerequisites
- `qemu-riscv64`
- `make`
- GCC cross-compiler for RISC-V (provided in the project VM)

### Compilation
```bash
make qemu

