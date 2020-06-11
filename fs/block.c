#include "block.h"
#include "bitmap.h"
#include "device.h"
#include <stdlib.h>

disk_ptr_t block_new()
{
    int length = (nblock + 31) / 32;
    uint_t *bmp = (uint_t *)malloc(length * 4);
    
    dev_read(SUPER_PTR + sizeof(super_block_t),
        (void *)bmp, length * 4);
    
    int idx = __find_empty(bmp, length, 1);
    if (idx >= 0) {
        __set_inuse(bmp, length, idx, 1);
        dev_write(SUPER_PTR + sizeof(super_block_t),
            (void *)bmp, length * 4);
    }

    free(bmp);
    return idx * BLOCK_SIZE;
}

void block_destroy(disk_ptr_t ptr)
{
    int idx = ptr / BLOCK_SIZE;
    int length = (nblock + 31) / 32;
    uint_t *bmp = (uint_t *)malloc(length * 4);
    
    dev_read(SUPER_PTR + sizeof(super_block_t),
        (void *)bmp, length * 4);
    
    __set_empty(bmp, length, idx, 1);

    dev_write(SUPER_PTR + sizeof(super_block_t),
        (void *)bmp, length * 4);
    
    free(bmp);
}

