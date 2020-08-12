
#include "vm/pagetable.h"
#include "threads/palloc.h"
#include "userprog/process.h"
#include <string.h>

void evict_frame (void);
frame_info_t *choose_frame_to_evict (void);
page_info_t *get_page_from_pagedir (void* page);
void *allocate_frame (void);
void free_page_info (page_info_t* page_info);
bool install_page_(struct thread *process, void *upage, void *kpage, bool writable);

// void *page_fault_handler (void *addr, bool user, bool writable) 
// {
//     //lock_acquire(&page_table_lock);
//     void *page = (void*)((uint32_t)addr & 0xfffff000);
//     page_info_t *page_info = get_page_from_pagedir(page);  // = process.pageinfodir[page]

//     if (page_info != NULL) // process.pageinfodir.contains(page): // Is In Swap
//     {
//         void *frame = allocate_frame(user);

//         frame_info_t *frame_info = fill_swap(page_info, frame);
//         ASSERT(frame_info);

//         push_back_to_frame_infos_map(frame_info);

//         struct list_elem *e;
//         for (e = list_begin(&(frame_info->owner_pages)); e != list_end(&(frame_info->owner_pages)); e = list_next(e))
//         {
//             page_info_t *owner_page = list_entry(e, page_info_t, elem);
//             bool success = pagedir_set_page(owner_page->process->pagedir, owner_page->page, frame, owner_page->writable);
//             ASSERT(success);
//         }
//         //lock_release(&page_table_lock);
//         return frame;
//     } 
//     else
//     {
//         //lock_release(&page_table_lock);
//         return page_installer(page, user, writable);
//     }
// }

page_info_t *page_register (void *page, bool writable)
{
    struct thread *process = thread_current();

    page_info_t *page_info = malloc(sizeof(page_info_t));
    page_info->process = process;
    page_info->page = page;
    page_info->writable = writable;
    page_info->owned_frame = NULL;
    page_info->mapid = -1;

    list_push_back(&(process->pageinfodir), &(page_info->pagedir_elem));
    return page_info;
}

void *page_installer (page_info_t *page_info) {
    // lock_acquire(&page_table_lock);
    void *frame = allocate_frame();

    frame_info_t *frame_info = new_instance(frame);
    push_back_to_frame_infos_map(frame_info);

    frame_info->owner_page = page_info;
    page_info->owned_frame = frame_info;

    install_page_(page_info->process, page_info->page, frame, page_info->writable);

    pagedir_set_accessed(page_info->process->pagedir, page_info->page, false);
	pagedir_set_dirty(page_info->process->pagedir, page_info->page, false);

    if (page_info->mapid != -1) {
        int index = page_info->index;
		struct file *file = page_info->file;
		file_seek(file, index * PGSIZE);
		file_read(file, frame, PGSIZE);
    }

    else fill_swap(page_info);

    // lock_release(&page_table_lock);
    return frame;
}

bool install_page_(struct thread *process, void *upage, void *kpage, bool writable) {
    return (pagedir_get_page(process->pagedir, upage) == NULL && pagedir_set_page(process->pagedir, upage, kpage, writable));
}

void *allocate_frame ()
{
    void *frame;
    frame = palloc_get_page(PAL_USER | PAL_ZERO);

    while (!frame) {
        evict_frame();
        frame = palloc_get_page(PAL_USER | PAL_ZERO);
    }

    return frame;
}

void evict_frame()
{
    struct list* frame_list = &(frame_infos_map.frame_infos);
    //ASSERT(list_size(frame_list) > 0);

    frame_info_t* chosen_frame = NULL;
    size_t frames_examined = 0;

    while (true)
    {
        if (frames_examined == list_size(frame_list))
        {
            chosen_frame = NULL;
            break;
        }

        frame_info_t* cur_frame = list_entry(list_pop_front(frame_list), frame_info_t, elem);
        page_info_t *owner_page = cur_frame->owner_page;

        if (!owner_page)
        {
            chosen_frame = cur_frame;
            break;
        }

        uint32_t* pagedir = owner_page->process->pagedir;

        if (!pagedir_is_accessed(pagedir, owner_page->page) && owner_page->writable)
        {
            chosen_frame = cur_frame;
            break;
        }
        else
        {
            list_push_back(frame_list, &cur_frame->elem);
            pagedir_set_accessed(pagedir, owner_page->page, false);
            frames_examined++;
        }
    }

    //ASSERT(chosen_frame);
    page_info_t *owner_page = chosen_frame->owner_page;
    //ASSERT(owner_page);

    if (owner_page->mapid != -1) {

        int index = owner_page->index;
		struct file *file = owner_page->file;
		file_seek(file, index * PGSIZE);
		file_write(file, chosen_frame->frame, PGSIZE);

    }
    else if (!save_swap(chosen_frame))
        PANIC("Couldn't evict frame");

    pagedir_clear_page(owner_page->process->pagedir, owner_page->page);
	palloc_free_page(chosen_frame->frame);
}

page_info_t *get_page_from_pagedir(void* page)
{
    struct list* pageinfodir = &(thread_current()->pageinfodir);
    struct list_elem* page_elem;
    for (page_elem = list_begin(pageinfodir); page_elem != list_end(pageinfodir); page_elem = list_next(page_elem))
    {
        page_info_t* cur_page = list_entry(page_elem, page_info_t, pagedir_elem); 
        if (page == cur_page->page)
            return cur_page;
    }
    return NULL;
}

void free_resources() { }