
#ifndef VM_PAGETABLE_H
#define VM_PAGETABLE_H

#include <stdbool.h>
#include "vm/pageinfo.h"
#include "vm/frameinfo.h"
#include "vm/swapstable.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"

page_info_t *page_register (void *page, bool writable);
void* page_installer (page_info_t *page_info);
page_info_t *get_page_from_pagedir(void* page);
void  free_resources (void);
static struct lock page_table_lock;

#endif /* vm/pagetable.h */
