#ifndef __fd_h__
#define __fd_h__

#include "fs.h"

struct __ptr_link_st {
    disk_ptr_t ptr;
    struct __ptr_link_st *next;
    struct __ptr_link_st *prev;
};

typedef struct __ptr_link_st __ptr_link_t;

struct file_desc_st {
    entry_t *e;
    long long offset;
    disk_ptr_t current;
    disk_ptr_t entry_ptr;
    __ptr_link_t *link;
    int flags;
};

typedef struct file_desc_st file_desc_t;

#define FD_COUNT    0x100
extern struct file_desc_st __g_fds[];

__ptr_link_t * __ptr_link_new();
void __ptr_link_destroy(__ptr_link_t *link);
void __ptr_link_load(__ptr_link_t *link, disk_ptr_t ptr);

int fd_new();
void fd_destroy(int fd);

void fd_bind(int fd, disk_ptr_t entry_ptr);

#endif