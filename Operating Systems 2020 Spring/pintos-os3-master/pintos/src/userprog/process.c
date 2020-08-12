#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

#include "vm/pageinfo.h"
#include "vm/frameinfo.h"
#include "vm/pagetable.h"

static thread_func start_process NO_RETURN;
static bool load(const char *cmdline, void (**eip)(void), void **esp);

static void load_arguments(char **argv, int argc, void **esp);
static int count_args(char *unparsedArgs);
static void parse_args(char **argv, char *unparsedArgs);
void free_file_descriptors(struct thread* cur);

/*
 * struct for passing parameters from
 * Parent Process to Child Process
 */
struct process_aux_t {
    // args
    char* file_name;
    struct process_info_t* process_info;
    // For communication
    struct semaphore* signaler;
    int success;
};

/* Starts a new thread running a user program loaded from
   FILENAME.  The new thread may be scheduled (and may even exit)
   before process_execute() returns.  Returns the new process's
   thread id, or TID_ERROR if the thread cannot be created. */
tid_t process_execute(const char *file_name_)
{
    tid_t tid;
    struct process_info_t* child_info = ProcessInfoInit();
    struct semaphore* child_process_signaler = (struct semaphore*) malloc(sizeof(struct semaphore));
    struct process_aux_t* aux = (struct process_aux_t*) malloc(sizeof(struct process_aux_t));
    size_t i, file_name_len = strlen(file_name_) + 1;
    char* file_name = (char *) malloc(file_name_len * sizeof(char));

    for (i = 0; i < file_name_len; i++)
        file_name[i] = file_name_[i];

    sema_init(child_process_signaler, 0);

    aux->file_name = file_name;
    aux->process_info = child_info;
    aux->signaler = child_process_signaler;

    tid = thread_create(file_name_, PRI_DEFAULT, start_process, aux);

    if (tid == TID_ERROR)
        free(child_info);
    else
    {
        sema_down(child_process_signaler);

        if (aux->success) {

            struct thread *curr_thread = thread_current();
            struct list *child_infos = curr_thread->child_infos;

            if (!child_infos) {
                struct thread* curr_thread = thread_current();
                curr_thread->child_infos = (struct list *) malloc(sizeof(struct list));
                list_init(curr_thread->child_infos);
                child_infos = curr_thread->child_infos;
            }

            list_push_back(child_infos, &(child_info->l_elem));
        }
        else tid = -1;
    }

    free(child_process_signaler);
    free(file_name);
    free(aux);

    return tid;
}

/* A thread function that loads a user process and starts it
   running. */
static void
start_process(void* vaux)
{
    struct thread* curr_thread = thread_current();

    curr_thread->child_infos = (struct list *) malloc(sizeof(struct list));
    list_init(curr_thread->child_infos);

    struct process_aux_t* aux = (struct process_aux_t*) vaux;

    // args
    char* file_name = aux->file_name;
    struct process_info_t* process_info = aux->process_info;

    /* Sets observable_tid for process_info */
    process_info->observable_tid = thread_tid();

    /* Set process_info to current thread */
    curr_thread->process_info = process_info;

    size_t i, len = strlen(file_name) + 1;
    char* unparsedArgs = (char*) malloc(len * sizeof(char));
    for (i = 0; i < len; i++)
        unparsedArgs[i] = file_name[i];

    int argc = count_args(unparsedArgs);
    char** arguments = (char**) malloc(argc * sizeof(char *));
    parse_args(arguments, unparsedArgs);

    /* Initialize interrupt frame and load executable. */
    struct intr_frame if_;
    memset(&if_, 0, sizeof if_);
    if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
    if_.cs = SEL_UCSEG;
    if_.eflags = FLAG_IF | FLAG_MBS;

    bool success = load(arguments[0], &if_.eip, &if_.esp);

    /* Signal Parent about success value */
    aux->success = success;
    sema_up(aux->signaler);

    /* If load failed, quit. */
    if (!success) {
        // free
        free(unparsedArgs);
        free(arguments);

        process_info->exit_code = -1; // set error code
        process_info->someone_died = 1; // dummy
        thread_exit();
    }

    load_arguments(arguments, argc, &if_.esp);

    // free
    free(unparsedArgs);
    free(arguments);

    /* Start the user process by simulating a return from an
     interrupt, implemented by intr_exit (in
     threads/intr-stubs.S).  Because intr_exit takes all of its
     arguments on the stack in the form of a `struct intr_frame',
     we just point the stack pointer (%esp) to our stack frame
     and jump to it. */
    asm volatile("movl %0, %%esp; jmp intr_exit"
                 :
                 : "g"(&if_)
                 : "memory");
    NOT_REACHED();
}

/* Waits for thread TID to die and returns its exit status.  If
   it was terminated by the kernel (i.e. killed due to an
   exception), returns -1.  If TID is invalid or if it was not a
   child of the calling process, or if process_wait() has already
   been successfully called for the given TID, returns -1
   immediately, without waiting.

   This function will be implemented in problem 2-2.  For now, it
   does nothing. */
int process_wait(tid_t child_tid)
{
    int exit_code = -1;

    struct thread *curr_thread = thread_current();
    struct list *child_infos = curr_thread->child_infos;

    if (child_infos) {
        struct list_elem *e = list_begin(child_infos);
        for (e = list_begin(child_infos); e != list_end(child_infos);
            e = list_next(e))
        {
            struct process_info_t *child_info = list_entry(e, struct process_info_t, l_elem);
            if (child_info->observable_tid == child_tid && child_info->observer_seen == 0)
            {
                // Wait for child...
                sema_down(&(child_info->wait_lock));
                exit_code = child_info->exit_code;
                child_info->observer_seen = 1;
                break;
            }
        }
    }
    return exit_code;
}

void print_exit_status(int exit_status) {

  const char* thread_name = thread_current()->name;
  size_t thread_name_len = strlen(thread_name);

  char* program_name = (char *) malloc(thread_name_len + 1);

  strlcpy(program_name, thread_name, thread_name_len + 1);

  size_t i;
  for (i = 0; i < thread_name_len; i++)
      if (program_name[i] == ' ') {
          program_name[i] = '\0';
          break;
      }

  printf("%s: exit(%d)\n", program_name, exit_status);

  free(program_name);
}

/* Free the current process's resources. */
void process_exit(void)
{
    struct thread *cur = thread_current();
    print_exit_status(cur->process_info->exit_code);

    free_resources ();

    uint32_t *pd;

    /* Destroy the current process's page directory and switch back
     to the kernel-only page directory. */
    pd = cur->pagedir;
    if (pd != NULL)
    {
        /* Correct ordering here is crucial.  We must set
         cur->pagedir to NULL before switching page directories,
         so that a timer interrupt can't switch back to the
         process page directory.  We must activate the base page
         directory before destroying the process's page
         directory, or our active page directory will be one
         that's been freed (and cleared). */
        cur->pagedir = NULL;
        pagedir_activate(NULL);
        pagedir_destroy(pd);
    }
    
    free_file_descriptors(cur);

    if (cur->thread_owned_exec != NULL)
        file_close(cur->thread_owned_exec);
}

void free_file_descriptors(struct thread* cur)
{
    int i = 0;
    struct file **file_arr = cur->fd_arr;
    for (; i < MAX_FD; i++)
    {
        if (file_arr[i] != NULL)
            file_close(file_arr[i]);
    }
}

/* Sets up the CPU for running user code in the current
   thread.
   This function is called on every context switch. */
void process_activate(void)
{
    struct thread *t = thread_current();

    /* Activate thread's page tables. */
    pagedir_activate(t->pagedir);

    /* Set thread's kernel stack for use in processing
     interrupts. */
    tss_update();
}

/* We load ELF binaries.  The following definitions are taken
   from the ELF specification, [ELF1], more-or-less verbatim.  */

/* ELF types.  See [ELF1] 1-2. */
typedef uint32_t Elf32_Word, Elf32_Addr, Elf32_Off;
typedef uint16_t Elf32_Half;

/* For use with ELF types in printf(). */
#define PE32Wx PRIx32 /* Print Elf32_Word in hexadecimal. */
#define PE32Ax PRIx32 /* Print Elf32_Addr in hexadecimal. */
#define PE32Ox PRIx32 /* Print Elf32_Off in hexadecimal. */
#define PE32Hx PRIx16 /* Print Elf32_Half in hexadecimal. */

/* Executable header.  See [ELF1] 1-4 to 1-8.
   This appears at the very beginning of an ELF binary. */
struct Elf32_Ehdr
{
    unsigned char e_ident[16];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
};

/* Program header.  See [ELF1] 2-2 to 2-4.
   There are e_phnum of these, starting at file offset e_phoff
   (see [ELF1] 1-6). */
struct Elf32_Phdr
{
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
};

/* Values for p_type.  See [ELF1] 2-3. */
#define PT_NULL 0           /* Ignore. */
#define PT_LOAD 1           /* Loadable segment. */
#define PT_DYNAMIC 2        /* Dynamic linking info. */
#define PT_INTERP 3         /* Name of dynamic loader. */
#define PT_NOTE 4           /* Auxiliary info. */
#define PT_SHLIB 5          /* Reserved. */
#define PT_PHDR 6           /* Program header table. */
#define PT_STACK 0x6474e551 /* Stack segment. */

/* Flags for p_flags.  See [ELF3] 2-3 and 2-4. */
#define PF_X 1 /* Executable. */
#define PF_W 2 /* Writable. */
#define PF_R 4 /* Readable. */

static bool setup_stack(void **esp);
static bool validate_segment(const struct Elf32_Phdr *, struct file *);
static bool load_segment(struct file *file, off_t ofs, uint8_t *upage,
                         uint32_t read_bytes, uint32_t zero_bytes,
                         bool writable);

/* Loads an ELF executable from FILE_NAME into the current thread.
   Stores the executable's entry point into *EIP
   and its initial stack pointer into *ESP.
   Returns true if successful, false otherwise. */
bool load(const char *file_name, void (**eip)(void), void **esp)
{
    struct thread *t = thread_current();
    struct Elf32_Ehdr ehdr;
    struct file *file = NULL;
    off_t file_ofs;
    bool success = false;
    int i;

    /* Allocate and activate page directory. */
    t->pagedir = pagedir_create();
    if (t->pagedir == NULL)
        goto done;
    process_activate();

    /* Open executable file.*/
    file = filesys_open(file_name);
    if (file == NULL)
    {
        printf("load: %s: open failed\n", file_name);
        goto done;
    }

    /* Read and verify executable header. */
    if (file_read(file, &ehdr, sizeof ehdr) != sizeof ehdr || memcmp(ehdr.e_ident, "\177ELF\1\1\1", 7) || ehdr.e_type != 2 || ehdr.e_machine != 3 || ehdr.e_version != 1 || ehdr.e_phentsize != sizeof(struct Elf32_Phdr) || ehdr.e_phnum > 1024)
    {
        printf("load: %s: error loading executable\n", file_name);
        goto done;
    }

    /* Deny write to current executable */
    file_deny_write(file);

    /* Read program headers. */
    file_ofs = ehdr.e_phoff;
    for (i = 0; i < ehdr.e_phnum; i++)
    {
        struct Elf32_Phdr phdr;

        if (file_ofs < 0 || file_ofs > file_length(file))
            goto done;
        file_seek(file, file_ofs);

        if (file_read(file, &phdr, sizeof phdr) != sizeof phdr)
            goto done;
        file_ofs += sizeof phdr;
        switch (phdr.p_type)
        {
        case PT_NULL:
        case PT_NOTE:
        case PT_PHDR:
        case PT_STACK:
        default:
            /* Ignore this segment. */
            break;
        case PT_DYNAMIC:
        case PT_INTERP:
        case PT_SHLIB:
            goto done;
        case PT_LOAD:
            if (validate_segment(&phdr, file))
            {
                bool writable = (phdr.p_flags & PF_W) != 0;
                uint32_t file_page = phdr.p_offset & ~PGMASK;
                uint32_t mem_page = phdr.p_vaddr & ~PGMASK;
                uint32_t page_offset = phdr.p_vaddr & PGMASK;
                uint32_t read_bytes, zero_bytes;
                if (phdr.p_filesz > 0)
                {
                    /* Normal segment.
                     Read initial part from disk and zero the rest. */
                    read_bytes = page_offset + phdr.p_filesz;
                    zero_bytes = (ROUND_UP(page_offset + phdr.p_memsz, PGSIZE) - read_bytes);
                }
                else
                {
                    /* Entirely zero.
                     Don't read anything from disk. */
                    read_bytes = 0;
                    zero_bytes = ROUND_UP(page_offset + phdr.p_memsz, PGSIZE);
                }
                if (!load_segment(file, file_page, (void *)mem_page,
                                  read_bytes, zero_bytes, writable))
                    goto done;
            }
            else
                goto done;
            break;
        }
    }

    /* Set up stack. */
    if (!setup_stack(esp))
        goto done;

    /* Start address. */
    *eip = (void (*)(void))ehdr.e_entry;

    success = true;

done:

    /*  We arrive here whether the load is successful or not.
        If exec was opened successfully, save it 
        Else close it */

    if (success)
        t->thread_owned_exec = file;
    else
        file_close(file);

    return success;
}

/* load() helpers. */

static bool install_page(void *upage, void *kpage, bool writable);

/* Checks whether PHDR describes a valid, loadable segment in
   FILE and returns true if so, false otherwise. */
static bool
validate_segment(const struct Elf32_Phdr *phdr, struct file *file)
{
    /* p_offset and p_vaddr must have the same page offset. */
    if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK))
        return false;

    /* p_offset must point within FILE. */
    if (phdr->p_offset > (Elf32_Off)file_length(file))
        return false;

    /* p_memsz must be at least as big as p_filesz. */
    if (phdr->p_memsz < phdr->p_filesz)
        return false;

    /* The segment must not be empty. */
    if (phdr->p_memsz == 0)
        return false;

    /* The virtual memory region must both start and end within the
     user address space range. */
    if (!is_user_vaddr((void *)phdr->p_vaddr))
        return false;
    if (!is_user_vaddr((void *)(phdr->p_vaddr + phdr->p_memsz)))
        return false;

    /* The region cannot "wrap around" across the kernel virtual
     address space. */
    if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
        return false;

    /* Disallow mapping page 0.
     Not only is it a bad idea to map page 0, but if we allowed
     it then user code that passed a null pointer to system calls
     could quite likely panic the kernel by way of null pointer
     assertions in memcpy(), etc. */
    if (phdr->p_vaddr < PGSIZE)
        return false;

    /* It's okay. */
    return true;
}

/* Loads a segment starting at offset OFS in FILE at address
   UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
   memory are initialized, as follows:

        - READ_BYTES bytes at UPAGE must be read from FILE
          starting at offset OFS.

        - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.

   The pages initialized by this function must be writable by the
   user process if WRITABLE is true, read-only otherwise.

   Return true if successful, false if a memory allocation error
   or disk read error occurs. */
static bool
load_segment(struct file *file, off_t ofs, uint8_t *upage,
             uint32_t read_bytes, uint32_t zero_bytes, bool writable)
{
    ASSERT((read_bytes + zero_bytes) % PGSIZE == 0);
    ASSERT(pg_ofs(upage) == 0);
    ASSERT(ofs % PGSIZE == 0);

    file_seek(file, ofs);
    while (read_bytes > 0 || zero_bytes > 0)
    {
        /* Calculate how to fill this page.
         We will read PAGE_READ_BYTES bytes from FILE
         and zero the final PAGE_ZERO_BYTES bytes. */
        size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
        size_t page_zero_bytes = PGSIZE - page_read_bytes;

        page_info_t *page_info = page_register(upage, writable);
        void *kpage = page_installer(page_info);

        if (kpage == NULL)
            return false;

        /* Load this page. */
        if (file_read(file, kpage, page_read_bytes) != (int)page_read_bytes)
        {
            // palloc_free_page(kpage);
            return false;
        }
        memset(kpage + page_read_bytes, 0, page_zero_bytes);

        /* Add the page to the process's address space. */
        // if (!install_page(upage, kpage, writable))
        // {
        //     palloc_free_page(kpage);
        //     return false;
        // }

        /* Advance. */
        read_bytes -= page_read_bytes;
        zero_bytes -= page_zero_bytes;
        upage += PGSIZE;
    }
    return true;
}

/* Create a minimal stack by mapping a zeroed page at the top of
   user virtual memory. */
static bool
setup_stack(void **esp)
{
    uint8_t *kpage;
    bool success = false;

    kpage = palloc_get_page(PAL_USER | PAL_ZERO);
    if (kpage != NULL)
    {
        success = install_page(((uint8_t *)PHYS_BASE) - PGSIZE, kpage, true);
        if (success)
            *esp = PHYS_BASE;
        else
            palloc_free_page(kpage);
    }
    return success;
}

/* Adds a mapping from user virtual address UPAGE to kernel
   virtual address KPAGE to the page table.
   If WRITABLE is true, the user process may modify the page;
   otherwise, it is read-only.
   UPAGE must not already be mapped.
   KPAGE should probably be a page obtained from the user pool
   with palloc_get_page().
   Returns true on success, false if UPAGE is already mapped or
   if memory allocation fails. */
static bool
install_page(void *upage, void *kpage, bool writable)
{
    struct thread *t = thread_current();
    return (pagedir_get_page(t->pagedir, upage) == NULL && pagedir_set_page(t->pagedir, upage, kpage, writable));
}

static void load_arguments(char **argv, int argc, void **esp)
{
    int i = 0;

    //argv[0][...] pointer
    char *ptArgs = *esp;
    for (i = argc - 1; i >= 0; i--)
    {
        *esp -= strlen(argv[i]) + 1;
        strlcpy(*esp, argv[i], PGSIZE);
    }

    //wordalign
    while (true)
    {
        *esp -= 1;
        if ((int)*esp & 0x00000003)
            *(char *)*esp = 0;
        else
            break;
    }

    //NULL sentinel
    *esp -= sizeof(char *);
    *((char **)*esp) = NULL;

    //push argument addresses backwards
    for (i = argc - 1; i >= 0; i--)
    {
        *esp -= sizeof(char *);
        ptArgs -= strlen(argv[i]) + 1;
        *((char **)*esp) = ptArgs;
    }

    //argv
    *esp -= sizeof(char **);
    *((char **)*esp) = *esp + sizeof(char **);

    //argc
    *esp -= sizeof(int);
    *((int *)*esp) = argc;

    //return
    *esp -= sizeof(void *);
    *((int *)*esp) = 0;
}

//counts arguments
static int count_args(char *unparsedArgs)
{
    int argc = 1;
    int i = 0;
    for (; unparsedArgs[i];)
        if (unparsedArgs[i] == ' ') 
        {
            while (unparsedArgs[++i] == ' ');
            argc++;
        } else i++;
    if(unparsedArgs[i-1]==' ') 
        argc--;
    return argc;
}

//parses args into an array of strings
static void parse_args(char **argv, char *unparsedArgs)
{
    int last = 0, argNum = 0, size = strlen(unparsedArgs), i = 0;
    for (i = 0; i <= size; i++) {
        if (unparsedArgs[i] == ' ' || unparsedArgs[i] == 0)
        {
            unparsedArgs[i] = 0;

            while (i < size && unparsedArgs[i + 1] == ' ')
                i++;

            argv[argNum] = &unparsedArgs[last];
            last = i + 1;
            argNum++;
        }
    }
}