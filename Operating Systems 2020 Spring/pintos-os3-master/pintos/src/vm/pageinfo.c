
#include "vm/pageinfo.h"

bool equals (page_info_t *self, page_info_t *other) 
{
    return self->process->tid == other->process->tid &&
           self->page == other->page;
}
