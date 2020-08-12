/*
#ifndef VM_MMF_H
#define VM_MMF_H

#include "threads/thread.h"

//TODO: include <syscall.h> instead of typedef?
typedef int mapid_t;

typedef struct{
    void* page;
    int fd;
    struct list_elem elem;
} mmf_tuple;

mapid_t mmap_handler(struct mmfs milf, int fd, void *addr);
void munmap_handler(struct mmfs milf, mapid_t mapid);
void fill(struct mmfs milf, void* frame, void *page);

#endif /* vm/mmf.h */
