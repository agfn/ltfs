#include "fs_api.h"
#include "path.h"
#include "device.h"
#include "fd.h"
#include "block.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define FILE_BLOCK_SIZE     (BLOCK_SIZE - sizeof(void *))
#define CURRENT_LIMIT(ptr)  (((ptr) & ~BLOCK_MASK) + FILE_BLOCK_SIZE - (ptr))

void __update_current(file_desc_t *_fd)
{
    int offset = _fd->offset;
    __ptr_link_t *l = _fd->link;

    while (offset > FILE_BLOCK_SIZE) {
        if (l->next) {
            l = l->next;
        } else {
            /* add one file block */
            l->next = __ptr_link_new();;
            l->next->next = NULL;
            l->next->prev = l;
            l->next->ptr = block_new();
            // _fd->e->file.size += FILE_BLOCK_SIZE;
            dev_write(l->ptr + FILE_BLOCK_SIZE, 
                (void *)&l->next->ptr, sizeof(disk_ptr_t));
            l = l->next;
        }
        offset -= FILE_BLOCK_SIZE;
    }

    _fd->current = l->ptr + offset;
}


int fs_create(const char *path, char type)
{
    int ret = -1;
    path_t *p = path_new(), *ptmp;
    entry_t e = { 0 };
    path_parse(p, path);
    ret = path_insert(getroot(), p);
    if (!ret) {
        ptmp = path_find(getroot(), p);
        if (ptmp) {
            dev_read(ptmp->ptr, (void *)&e, sizeof(e));
            e.type = type;

            if (e.type == ENTRY_TYPE_FILE) {
                e.file.ptr = block_new();
                e.file.size = BLOCK_SIZE - 8;
                disk_ptr_t zero = 0;
                dev_write(e.file.ptr + BLOCK_SIZE - 8, 
                    &zero, 8);
            }

            dev_write(ptmp->ptr, (void *)&e, sizeof(e));
        } else ret = -1;
    }
    path_destroy(p);
    return ret;
}

int fs_open(const char *path, int flags)
{
    int fd = -1;
    struct path_st *p = path_new();
    file_desc_t *_fd;
    path_parse(p, path);
    path_t *p_find = path_find(getroot(), p);
    if (p_find) {
        fd = fd_new();
        fd_bind(fd, p_find->ptr);
        _fd = &__g_fds[fd];
        _fd->flags = flags;

        if (_fd->flags == _O_WONLY) {
            _fd->e->file.size = 0;
            dev_write(_fd->entry_ptr, (void *)_fd->e, sizeof(entry_t));
        }

        if (_fd->flags == _O_APPEND) {
            _fd->offset = _fd->e->file.size;
            __update_current(_fd);
        }
    }
    path_destroy(p);
    return fd;
}


int fs_seek(int fd, long long offset, int whence)
{
    if (fd < 0 || fd >= FD_COUNT) 
        return -1;
    
    file_desc_t *_fd = &__g_fds[fd];
    
    switch (whence)
    {
    case _SEEK_CUR:
        _fd->offset += offset;
        break;
    case _SEEK_SET:
        _fd->offset = offset;
        break;
    default:
        break;
    }
    __update_current(_fd);
}

int __internal_rw(file_desc_t *_fd, void *buf, int size, char op)
{
    char *rwp = (char *)buf;
    __ptr_link_t *l = _fd->link;
    int limit = CURRENT_LIMIT(_fd->current);
    int left = size, ret = 0;
    
    while (left) {
        if (left <= limit) {
            if (op == 'r') {
                if (_fd->offset >= _fd->e->file.size)
                    break;
                _fd->current += dev_read(_fd->current, (void *)rwp, left);
            } else if (op == 'w') {
                _fd->current += dev_write(_fd->current, (void *)rwp, left);
            } else {
                assert (0);
            }
            _fd->offset += left;
            ret += left;
            break;
        } else {
            if (op == 'r') {
                if (_fd->offset >= _fd->e->file.size)
                    break;
                _fd->current += dev_read(_fd->current, (void *)rwp, limit);
            } else if (op == 'w') {
                _fd->current += dev_write(_fd->current, (void *)rwp, limit);
            } else {
                assert (0);
            }
            _fd->offset += limit;
            ret += limit;
            left -= limit;
        }

        // limit & ptr 
        if (l->next) {
            l = l->next;
        } else {
            /* add one file block */
            l->next = __ptr_link_new();;
            l->next->next = NULL;
            l->next->prev = l;
            l->next->ptr = block_new();
            // _fd->e->file.size += FILE_BLOCK_SIZE;
            // dev_write(_fd->entry_ptr, (void *)_fd->e, sizeof(entry_t));
            dev_write(l->ptr + FILE_BLOCK_SIZE, 
                (void *)&l->next->ptr, sizeof(disk_ptr_t));
            l = l->next;
        }
        _fd->current = l->ptr;
        limit = CURRENT_LIMIT(_fd->current);
    }
    if (_fd->flags == _O_APPEND || _fd->flags == _O_WONLY) {
        if (_fd->offset > _fd->e->file.size) {
            _fd->e->file.size = _fd->offset;
            dev_write(_fd->entry_ptr, (void *)_fd->e, sizeof(entry_t));
        }
    }

    return ret;
}

int fs_write(int fd, void *buf, int size)
{
    if (fd < 0 || fd >= FD_COUNT) 
        return -1;
    
    file_desc_t *_fd = &__g_fds[fd];
    if (!_fd->e || _fd->e->type != ENTRY_TYPE_FILE)
        return -1;
    
    if (_fd->flags != _O_WONLY && _fd->flags != _O_APPEND) {
        return -1;
    }

    return __internal_rw(_fd, buf, size, 'w');   
}

int fs_read(int fd, void *buf, int size)
{
    if (fd < 0 || fd >= FD_COUNT) 
        return -1;
    
    file_desc_t *_fd = &__g_fds[fd];
    if (!_fd->e || _fd->e->type != ENTRY_TYPE_FILE)
        return -1;
    
    if (_fd->flags != _O_RONLY) 
        return -1;

    return __internal_rw(_fd, buf, size, 'r');
}

int fs_rename(const char *path, const char *name)
{
    struct path_st p;
    path_parse(&p, path);
    return path_rename(getroot(), &p, name);
}

int fs_close(int fd)
{
    if (fd < 0 || fd >= FD_COUNT) 
        return -1;
    fd_destroy(fd);
    return 0;
}

int fs_delete(const char *path)
{
    int err = 0;
    entry_t e;
    struct path_st *p = path_new();
    path_parse(p, path);
    
    path_t * in_index_p = path_find(getroot(), p);
    if (in_index_p) {
        dev_read(in_index_p->ptr, (void *)&e, sizeof(e));
        if (e.type == ENTRY_TYPE_FILE) {
            disk_ptr_t ptr = e.file.ptr;
            while (ptr) {
                block_destroy(ptr);
                dev_read(ptr + FILE_BLOCK_SIZE, (void *)&ptr, 8);
            }
        }
        err = path_remove(getroot(), p);
    } else err = -1;

    return err;
}


void __set_info_by_ptr(struct fs_info_st *info, disk_ptr_t ptr)
{
    entry_t e;
    dev_read(ptr, (void *)&e, sizeof(e));

    strncpy(info->name, e.name, MAX_NAME_LEN);
    info->entry_ptr = ptr;
    if (e.type == ENTRY_TYPE_FILE) {
        info->type = 'f';
        info->size = e.file.size;
    } else if (e.type == ENTRY_TYPE_DIR) {
        info->type = 'd';
        info->size = sizeof(e);
    }

#define __extract_permission(per, p) \
    do { \
        if ((per) & 1) (p)[0] = 'r'; else (p)[0] = '-'; \
        if ((per) & 2) (p)[1] = 'w'; else (p)[1] = '-'; \
        if ((per) & 4) (p)[2] = 'x'; else (p)[2] = '-'; \
    } while (0)

        for (int i = 0; i < 3; i++)
            __extract_permission(e.permission[i], &info->permission[3*i]);
}

int fs_info(const char *path, struct fs_info_st *info)
{
    int err = 0;
    entry_t e;
    path_t *p = path_new();
    path_parse(p, path);
    path_t *pfind = path_find(getroot(), p);

    if (pfind)
        __set_info_by_ptr(info, pfind->ptr);
    else
        err = -1;
    return err;
}

int fs_info_child(struct fs_info_st *info, struct fs_info_st *child)
{
    int err = 0;
    entry_t e;
    dev_read(info->entry_ptr, (void *)&e, sizeof(e));

    if (e.type != ENTRY_TYPE_DIR) 
        err = -1;

    if (e.dir.sub_entry)
        __set_info_by_ptr(child, e.dir.sub_entry);
    else 
        err = -1;

    return err;
}

int fs_info_next(struct fs_info_st *info, struct fs_info_st *next)
{
    int err = 0;
    entry_t e;
    dev_read(info->entry_ptr, (void *)&e, sizeof(e));

    if (e.next)
        __set_info_by_ptr(next, e.next);
    else 
        err = -1;

    return err;
}
