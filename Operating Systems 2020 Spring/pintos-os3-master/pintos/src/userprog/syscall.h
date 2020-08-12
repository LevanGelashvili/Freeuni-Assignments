#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

typedef int mapid_t;
#define MAP_FAILED ((mapid_t) -1)

#include "threads/interrupt.h"

void syscall_init (void);
void exit_process (struct intr_frame *f, int exit_code);

#endif /* userprog/syscall.h */
