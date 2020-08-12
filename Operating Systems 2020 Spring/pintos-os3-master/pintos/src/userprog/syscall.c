#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "pagedir.h"
#include "threads/vaddr.h"
#include "process.h"
#include <string.h>
#include "threads/malloc.h"

#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"
#include "vm/pageinfo.h"
#include "vm/pagetable.h"
#include "vm/frameinfo.h"

#define unused __attribute__((unused))

/* Global lock to synchronize the whole filesystem */
static struct lock file_lock_global;

static void syscall_handler(struct intr_frame *);
void syscall_wait_handler(struct intr_frame *f);
void syscall_create_handler(struct intr_frame *f, char *name, unsigned size);
void syscall_remove_handler(struct intr_frame *f, char *name);
void syscall_open_handler(struct intr_frame *f, char *name);
void syscall_filesize_handler(struct intr_frame *f, int fd);
void syscall_read_handler(struct intr_frame *f, int fd, void *buffer, unsigned size);
void syscall_write_handler(struct intr_frame *f, int fd, void *buffer, unsigned size);
void syscall_seek_handler(unused struct intr_frame *f, int fd, unsigned position);
void syscall_tell_handler(struct intr_frame *f, int fd);
void syscall_close_handler(unused struct intr_frame *f, int fd);
void syscall_mmap_handler(struct intr_frame *f, int fd, void* addr);
void syscall_munmap_handler(struct intr_frame *f, mapid_t mapping);

#define POINTS_TO_STRING -1
void *assert_pointer(void *ptr, int bytes_to_check);
void terminate_if_invalid(void *ptr);

void syscall_init(void)
{
    lock_init(&file_lock_global);
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

#define INVALID_PTR(ptr, pagedir) (ptr == NULL || !is_user_vaddr(ptr) || pagedir_get_page(pagedir, ptr) == NULL )

/* Terminates thread if invalid pointer was found */
void terminate_if_invalid(void *ptr)
{
    if (INVALID_PTR(ptr, thread_current()->pagedir))
    {
        thread_signal_observer(-1);
        thread_exit();
    }
}

static void
syscall_handler(struct intr_frame *f)
{
    uint32_t *args = ((uint32_t *)f->esp);
    int sys_call = *(int *)assert_pointer(args, sizeof(uint32_t));

    char *str;
    int integer;
    unsigned u_integer;
    void *v_ptr;

    thread_current()->stack = (uint8_t *) f->esp;

    switch (sys_call)
    {
    case SYS_EXIT:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        exit_process(f, integer);
        break;

    case SYS_HALT:;
        shutdown_power_off();
        break;

    case SYS_EXEC:;
        str = *(char **)assert_pointer(args + 1, POINTS_TO_STRING);
        *(tid_t *)(&(f->eax)) = process_execute(str);
        break;

    case SYS_WAIT:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        *(int *)(&(f->eax)) = process_wait(integer);
        break;

    case SYS_PRACTICE:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        f->eax = integer + 1;
        break;

    case SYS_CREATE:;
        str = *(char **)assert_pointer(args + 1, POINTS_TO_STRING);
        unsigned size = *(unsigned *)assert_pointer(args + 2, sizeof(uint32_t));
        syscall_create_handler(f, str, size);
        break;

    case SYS_REMOVE:;
        str = *(char **)assert_pointer(args + 1, POINTS_TO_STRING);
        syscall_remove_handler(f, str);
        break;

    case SYS_OPEN:;
        str = *(char **)assert_pointer(args + 1, POINTS_TO_STRING);
        syscall_open_handler(f, str);
        break;

    case SYS_FILESIZE:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        syscall_filesize_handler(f, integer);
        break;

    case SYS_READ:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        u_integer = *(unsigned *)assert_pointer(args + 3, sizeof(uint32_t));
        v_ptr = (void *) args[2];

        terminate_if_invalid(v_ptr);
        terminate_if_invalid((char*)v_ptr + u_integer);

        syscall_read_handler(f, integer, v_ptr, u_integer);
        break;

    case SYS_WRITE:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        u_integer = *(unsigned *)assert_pointer(args + 3, sizeof(uint32_t));
        v_ptr = (void *) args[2];

        terminate_if_invalid(v_ptr);
        terminate_if_invalid((char*)v_ptr + u_integer);

        syscall_write_handler(f, integer, v_ptr, u_integer);
        break;

    case SYS_SEEK:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        u_integer = *(unsigned *)assert_pointer(args + 2, sizeof(uint32_t));
        syscall_seek_handler(f, integer, u_integer);
        break;

    case SYS_TELL:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        syscall_tell_handler(f, integer);
        break;

    case SYS_CLOSE:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        syscall_close_handler(f, integer);
        break;

    case SYS_MMAP:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        v_ptr = (void *) args[2];

        syscall_mmap_handler(f, integer, v_ptr);
        break;

    case SYS_MUNMAP:;
        integer = *(int *)assert_pointer(args + 1, sizeof(uint32_t));
        
        syscall_munmap_handler(f, (mapid_t)integer);
        break;

    default:
        break;
    }
}


void syscall_mmap_handler(struct intr_frame *f, int fd, void* ptr)
{
    struct thread *td = thread_current();

    if (fd <= 1 || fd >= td->last_fd || (size_t)ptr % PGSIZE || !ptr)
    {
        f->eax = MAP_FAILED;
    }
    else
    {
        struct file *file = file_reopen(td->fd_arr[fd]);
        off_t length = file_length(file);

        if (length <= 0 || (((char *) ptr + length) >= pg_round_down(td->stack)))
        {
            f->eax = MAP_FAILED;
            return;
        }

        off_t diff = (PGSIZE - (length % PGSIZE));

        if (diff != 0) {
            char *buffer = malloc(length + diff);
            file_read(file, buffer, length);
            memset(buffer + length, 0, diff);
            length += diff;
            file_write(file, buffer, length);
        }

        void *start = ptr;
        void *finish = (char *)ptr + length;

        struct list* pageinfodir = &(td->pageinfodir);
        struct list_elem* page_elem;
        for (page_elem = list_begin(pageinfodir); page_elem != list_end(pageinfodir); page_elem = list_next(page_elem))
        {
            page_info_t* curr_page = list_entry(page_elem, page_info_t, pagedir_elem);
            void *page = curr_page->page;
            if (page >= start && page <= finish) {
                f->eax = MAP_FAILED;
                return;
            }
        }

        int mapid = (int) ptr;
        int index;
        char *page;
        for (page = start, index = 0; page < finish; page += PGSIZE, index++) {

            page_info_t *page_info = page_register(page, true);
            page_info->file  = file;
            page_info->mapid = mapid;
            page_info->index = index;
        }
        f->eax = mapid;
    }
}

void syscall_munmap_handler(unused struct intr_frame *f, mapid_t mapping)
{
    struct thread *td = thread_current();
    struct list* pageinfodir = &(td->pageinfodir);
    struct list_elem* page_elem;
    for (page_elem = list_begin(pageinfodir); page_elem != list_end(pageinfodir); page_elem = list_next(page_elem))
    {
        page_info_t* page_info = list_entry(page_elem, page_info_t, pagedir_elem);
        if (page_info->mapid == mapping && pagedir_is_dirty(td->pagedir, page_info->page)) {
            int index = page_info->index;
            struct file *file = page_info->file;
            frame_info_t *owner_frame = (frame_info_t *) page_info->owned_frame;
            file_seek(file, index * PGSIZE);
            file_write(file, owner_frame->frame, PGSIZE);
            pagedir_clear_page(td->pagedir, page_info->page);
        }
    }
}

void syscall_create_handler(struct intr_frame *f, char *name, unsigned size)
{
    lock_acquire(&file_lock_global);
    f->eax = filesys_create(name, size);
    lock_release(&file_lock_global);
}

void syscall_remove_handler(struct intr_frame *f, char *name)
{
    lock_acquire(&file_lock_global);
    f->eax = filesys_remove(name);
    lock_release(&file_lock_global);
}

void syscall_open_handler(struct intr_frame *f, char *name)
{
    lock_acquire(&file_lock_global);
    struct file *fileptr = filesys_open(name);
    if (fileptr)
    {
        struct thread *td = thread_current();
        int i = 0;
        for (i = 2; i < td->last_fd; i++)
            if (fileptr == td->fd_arr[i])
            {
                f->eax = -1;
                break;
            }
        f->eax = td->last_fd;
        td->fd_arr[td->last_fd++] = fileptr;
    }
    else
        f->eax = -1;
    lock_release(&file_lock_global);
}

void syscall_filesize_handler(struct intr_frame *f, int fd)
{
    lock_acquire(&file_lock_global);

    struct thread *td = thread_current();

    f->eax = -1;
    if (fd > 1 && fd < td->last_fd) {
        struct file *fileptr = td->fd_arr[fd];
        f->eax = file_length(fileptr);
    }

    lock_release(&file_lock_global);
}

void syscall_read_handler(struct intr_frame *f, int fd, void *buffer, unsigned size)
{
    lock_acquire(&file_lock_global);
    struct thread *td = thread_current();
    f->eax = -1;

    if (fd == 0) {
        unsigned i;
        for (i = 0; i < size; i++)
            ((char *)buffer)[i] = input_getc();
        f->eax = size;
    } else if (fd > 1 && fd < td->last_fd) {
        struct file *fileptr = td->fd_arr[fd];
        f->eax = file_read(fileptr, buffer, size);
    }

    lock_release(&file_lock_global);
}

void syscall_write_handler(struct intr_frame *f, int fd, void *buffer, unsigned size)
{
    lock_acquire(&file_lock_global);
    struct thread *td = thread_current();
    f->eax = -1;

    if (fd == 1) {
        putbuf(buffer, size);
        f->eax = size;
    } else if (fd > 1 && fd < td->last_fd) {
        struct thread *td = thread_current();
        struct file *fileptr = td->fd_arr[fd];
        f->eax = file_write(fileptr, buffer, size);
    }

    lock_release(&file_lock_global);
}

void syscall_seek_handler(unused struct intr_frame *f, int fd, unsigned position)
{
    lock_acquire(&file_lock_global);

    struct thread *td = thread_current();
    struct file *fileptr = td->fd_arr[fd];

    if (fd > 1 && fd < td->last_fd)
        file_seek(fileptr, position);

    lock_release(&file_lock_global);
}

void syscall_tell_handler(struct intr_frame *f, int fd)
{
    lock_acquire(&file_lock_global);

    struct thread *td = thread_current();

    f->eax = -1;
    if (fd > 1 && fd < td->last_fd) {
        struct file *fileptr = td->fd_arr[fd];
        f->eax = file_tell(fileptr);
    }

    lock_release(&file_lock_global);
}

void syscall_close_handler(unused struct intr_frame *f, int fd)
{
    lock_acquire(&file_lock_global);
    struct thread *td = thread_current();
    if (fd < 2 || fd >= MAX_FD)
    {
        lock_release(&file_lock_global);
        thread_signal_observer(-1);
        thread_exit();
    }
    struct file *fileptr = td->fd_arr[fd];
    if (td->fd_arr[fd])
    {
        file_close(fileptr);
        td->fd_arr[fd] = NULL;
    }
    lock_release(&file_lock_global);
}

/*  Asserts every byte of pointer's validity and returns if for later use 
    POINTS_TO_STRING must be passed if points to string, else number of bytes to check */

void *assert_pointer(void *ptr, int bytes_to_check)
{
    int i;

    if (bytes_to_check == POINTS_TO_STRING)
    {
        char *char_ptr = *(char **)ptr;
        terminate_if_invalid(char_ptr);

        for (i = 0; char_ptr[i] != '\0'; i++)
        {
            terminate_if_invalid(char_ptr + i);
        }
    }
    else
    {
        for (i = 0; i < bytes_to_check; i++)
        {
            void *cur_ptr = (void *)((char *)ptr + i);
            terminate_if_invalid(cur_ptr);
        }
    }

    return ptr;
}

void exit_process (struct intr_frame *f, int exit_code) {
    thread_current()->process_info->exit_code = exit_code;
    f->eax = exit_code;
    thread_signal_observer(exit_code);
    thread_exit();
}
