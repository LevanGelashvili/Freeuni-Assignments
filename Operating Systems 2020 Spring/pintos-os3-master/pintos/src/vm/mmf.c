// #include "mmf.h"
// mapid_t compute_finish_page(mapid_t start_page, int fd);
// //TODO KRAWA:
// //TODO: evicti

// mapid_t mmap_handler(struct mmfs milf, int fd, void *addr){
//     /*
//         if invalid(): // TODO
//            return -1

//         copy_fd = file_reopen(fd)
//         if cant reopen:
//            return -1

//         new_start_page  = addr
//         new_finish_page = compute_finish_page(new_start_page, copy_fd)

//         for start_page, fd in mmfs:
//             finish_page = compute_finish_page(start_page, fd)

//             if [new_start_page, new_finish_page] overlaps with [start_page, finish_page]:
//                 return -1

//         mmfs.append( (new_start_page, copy_fd) )

//         return new_start_page // mapid_t

//     */
//     return 0;
// }

// void munmap_handler(struct mmfs milf, mapid_t mapid){
//     /*
//         for start_page, fd in mmfs:
//             if start_page == mapid:
//                mmfs.remove(start_page)
//                // TODO: Save segments which are in RAM

//                // i i i <- ram
//                // iiiii <- file

//                close(fd)
//                remove pages from pagedir if exists
//     */
// }

// void fill(struct mmfs milf, void* frame, void *page){
//     /*
//         for start_page, fd in mmfs:
//         finish_page = compute_finish_page(start_page, fd)

//         if page in [start_page, finish_page]:

//             start  = page - start_page
//             finish = start + PAGESIZE
//             *frame = fd[start : finish]

//             return
//     */
// }

// mapid_t compute_finish_page(mapid_t start_page, int fd){
//     return 0;

//     //return (start_page + file_size(fd)) & -4096
// }