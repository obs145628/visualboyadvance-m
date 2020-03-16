#ifndef SYSCALL_H
#define SYSCALL_H

// Whole file added for syscall

#define SYSCALL_TRIGGER_ADDR (0x06000250)
#define SYSCALL_ARGS_ADDR (0x06000260)

// Function called when the syscall addr is trigerred
// Contains code that do the actual syscall
void run_syscall();

#endif // SYSCALL_H
