
#ifndef VM_SWAPSTABLE_H
#define VM_SWAPSTABLE_H

#include <stdbool.h>
#include <bitmap.h>
#include <list.h>
#include "vm/frameinfo.h"
#include "vm/pageinfo.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "devices/block.h"

#define BLOCKS_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

typedef struct {
    frame_info_t** swaps;
    int swap_size;
    struct lock swap_lock;
} swaps_table_t;

void init_swap (void);
bool save_swap (frame_info_t *frame_info);
bool fill_swap (page_info_t *page_info);
bool remove_swap (frame_info_t *frame_info);

#endif
