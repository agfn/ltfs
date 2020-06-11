#include "fd.h"
#include "bitmap.h"
#include "device.h"
#include "entry.h"
#include <stdlib.h>
#include <string.h>

struct file_desc_st __g_fds[FD_COUNT];
uint_t fd_bmp[8];


__ptr_link_t * __ptr_link_new()
{
    return (__ptr_link_t *)malloc(sizeof(__ptr_link_t));
}


void __ptr_link_destroy(__ptr_link_t *link)
{
    __ptr_link_t *tofree;
    while (link) {
        tofree = link;
        link = link->next;
        free(tofree);
    }
}

void __ptr_link_load(__ptr_link_t *link, disk_ptr_t ptr)
{
    link->ptr = ptr;
    ptr += BLOCK_SIZE - 8;
    dev_read(ptr, &ptr, 8);
    if (ptr) {
        link->next = __ptr_link_new();
        link->next->prev = link;
        __ptr_link_load(link->next, ptr);
    } else {
        link->next = NULL;
    }
}

int fd_new()
{
    int fd = __find_empty(fd_bmp, 8, 1);
    if (fd >= 0) {
        __set_inuse(fd_bmp, 8, fd, 1);
        memset(&__g_fds[fd], 0, sizeof(file_desc_t));
    }

    return fd;
}

void fd_destroy(int fd)
{
    if (__g_fds[fd].e) {
        free(__g_fds[fd].e);
    }

    if (__g_fds[fd].link)
        __ptr_link_destroy(__g_fds[fd].link);
    
    __set_empty(fd_bmp, 8, fd, 1);
}


void fd_bind(int fd, disk_ptr_t entry_ptr)
{
    file_desc_t *_fd = &__g_fds[fd];
    entry_t *e = (entry_t *)malloc(sizeof(entry_t));
    dev_read(entry_ptr, (void *)e, sizeof(entry_t));

    _fd->entry_ptr = entry_ptr;
    _fd->e = e;
    _fd->flags = 0;
    _fd->offset = 0;
    
    if (_fd->e->type == ENTRY_TYPE_FILE) {
        _fd->link = __ptr_link_new();
        _fd->link->prev = NULL;
        __ptr_link_load(_fd->link, _fd->e->file.ptr);
        _fd->current = _fd->link->ptr;
    }
}

