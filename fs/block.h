#ifndef __block_h__
#define __block_h__

#include "fs.h"

disk_ptr_t block_new();
void block_destroy(disk_ptr_t ptr);

#endif