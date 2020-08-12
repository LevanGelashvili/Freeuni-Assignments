
#ifndef VM_PAGEINFO_H
#define VM_PAGEINFO_H

#include <stdbool.h>
#include <list.h>
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"

typedef struct {

    struct thread *process;
    void *page;
    bool writable;

    struct list_elem elem;
    struct list_elem pagedir_elem;
    void *owned_frame;

    int mapid;
    struct file *file;
    int index;

} page_info_t;

bool equals (page_info_t *self, page_info_t *other);

#endif /* vm/pageinfo.h */
