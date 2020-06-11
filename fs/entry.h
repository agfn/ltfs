#ifndef __entry_h__
#define __entry_h__
#include "fs.h"


disk_ptr_t entry_new();
int entry_destroy(disk_ptr_t ptr);
int entry_insert(disk_ptr_t parent, disk_ptr_t child);

int entry_rm_child(disk_ptr_t parent, disk_ptr_t child_next);
int entry_rm_next(disk_ptr_t ptr, disk_ptr_t next_next);

void entry_rename(disk_ptr_t ptr, const char *name);

#endif