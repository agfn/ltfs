#ifndef __device_h__
#define __device_h__

#include "path.h"

extern long long nblock;

path_t * getroot();
int dev_write(disk_ptr_t ptr, void *buf, int size);
int dev_read(disk_ptr_t ptr, void *buf, int size);

#endif