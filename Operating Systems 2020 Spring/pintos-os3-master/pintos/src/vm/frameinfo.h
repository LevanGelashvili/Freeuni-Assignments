
#ifndef VM_FRAMEINFO_H
#define VM_FRAMEINFO_H

#include <list.h>
#include <stdbool.h>
#include "threads/synch.h"
#include "vm/pageinfo.h"

typedef struct {
    struct lock lock;
    page_info_t *owner_page;
    void *frame;
    struct list_elem elem;
} frame_info_t;

frame_info_t* new_instance (void *frame);

/* Static Part */

typedef struct {
    struct lock lock;
    struct list frame_infos;
} frame_infos_map_t;

frame_infos_map_t frame_infos_map; // [ frame_info ]

void init_frame_infos_map (void);
void push_back_to_frame_infos_map (frame_info_t *frame_info);
bool remove_from_frame_infos_map (frame_info_t *frame_info);

#endif
