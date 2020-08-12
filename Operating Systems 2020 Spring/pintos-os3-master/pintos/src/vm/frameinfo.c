
#include "vm/frameinfo.h"

frame_info_t* new_instance(void *frame) 
{
    frame_info_t* frame_info;
    frame_info = malloc(sizeof(frame_info_t));

    lock_init(&(frame_info->lock));
    frame_info->frame = frame;

    return frame_info;
}

// void add_owner (frame_info_t *frame_info, page_info_t *page_info) {
//     lock_acquire(&(frame_info->lock));
//     list_push_back(&(frame_info->owner_pages), &(page_info->elem));
//     lock_release(&(frame_info->lock));
// }

/* Static Part */

void init_frame_infos_map (void) {
    list_init(&(frame_infos_map.frame_infos));
    lock_init(&(frame_infos_map.lock));
}

void push_back_to_frame_infos_map (frame_info_t *frame_info) {
    lock_acquire(&frame_infos_map.lock);
    list_push_back(&(frame_infos_map.frame_infos), &(frame_info->elem));
    lock_release(&frame_infos_map.lock);
}

bool remove_from_frame_infos_map (frame_info_t *frame_info) {
    lock_acquire(&frame_infos_map.lock);

    struct list* frame_infos = &(frame_infos_map.frame_infos);
    struct list_elem* e;
    for (e = list_begin(frame_infos); e != list_end(frame_infos); e = list_next(e))
    {
        frame_info_t* curr_frame_info = list_entry(e, frame_info_t, elem);
        if (curr_frame_info == frame_info) {
            list_remove(e);
            lock_release(&frame_infos_map.lock);
            return true;
        }
    }

    lock_release(&frame_infos_map.lock);
    return false;
}
