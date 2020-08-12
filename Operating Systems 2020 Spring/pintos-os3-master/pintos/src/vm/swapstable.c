
#include "vm/swapstable.h"

static swaps_table_t swap_table;
static struct block *block_device;

void init_swap()
{
    lock_init(&swap_table.swap_lock);
    block_device = block_get_role(BLOCK_SWAP);
    
    swap_table.swap_size = block_size(block_device) / BLOCKS_PER_PAGE;
    swap_table.swaps = malloc(swap_table.swap_size * sizeof(frame_info_t*));
    //ASSERT(swap_table.swaps != NULL);

    int i;
    for (i = 0; i < swap_table.swap_size; i++)
        swap_table.swaps[i] = NULL;    
}

bool save_swap(frame_info_t *frame_info)
{
    lock_acquire(&swap_table.swap_lock);

    int i;
    for (i = 0; i < swap_table.swap_size; i++)
    {
        if (swap_table.swaps[i] == NULL)
        {
            swap_table.swaps[i] = frame_info;
            
            block_sector_t free_sector = i * BLOCKS_PER_PAGE;
            for (i = 0; i < BLOCKS_PER_PAGE; i++)
            {
                block_write(block_device, free_sector + i, frame_info->frame + (i * BLOCKS_PER_PAGE));
            }

            lock_release(&swap_table.swap_lock);
            return true;
        }
    }

    lock_release(&swap_table.swap_lock);
    return false;
}

bool fill_swap(page_info_t *page_info)
{
    void *frame = page_info->owned_frame;
    lock_acquire(&swap_table.swap_lock);

    int i;
    for (i = 0; i < swap_table.swap_size; i++)
    {
        if (swap_table.swaps[i] == NULL) continue;
        frame_info_t *frame_info = swap_table.swaps[i];

        page_info_t *owner_page = frame_info->owner_page;

        if (owner_page == page_info)
        {
            block_sector_t free_sector = i * BLOCKS_PER_PAGE;
            for (i = 0; i < BLOCKS_PER_PAGE; i++)
            {
                block_read(block_device, free_sector + i, frame + (i * BLOCKS_PER_PAGE));
            }

            swap_table.swaps[i] = NULL;
            lock_release(&swap_table.swap_lock);
            return true;
        }
    }

    lock_release(&swap_table.swap_lock);
    return false;
}

bool remove_swap (frame_info_t *frame_info) {

    lock_acquire(&(swap_table.swap_lock));
    int i;
    for (i = 0; i < swap_table.swap_size; i++)
        if (swap_table.swaps[i] == frame_info)
        {
            swap_table.swaps[i] = NULL;
            lock_release(&(swap_table.swap_lock));
            return true;
        }

    lock_release(&(swap_table.swap_lock));
    return false;
}
