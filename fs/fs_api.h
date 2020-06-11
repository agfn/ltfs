#ifndef __fs_api_h__
#define __fs_api_h__

#include "fs.h"

#if defined (__cplusplus)
extern "C" {
#endif

#define DEV_NOT_USABLE      1
#define PARENT_DIR_NOT_FOUND    2
#define UNSUPPORTED_NAME    3

struct fs_info_st {
    char name[MAX_NAME_LEN];
    long long size;
    disk_ptr_t entry_ptr;
    char type;
    char permission[9];
    char reserve[6];
};

typedef struct fs_info_st fs_info_t;

#define _O_RONLY        0
#define _O_WONLY        1
#define _O_APPEND       2

#define _SEEK_SET       0
#define _SEEK_CUR       1

int fs_create(const char *path, char type);
int fs_open(const char *path, int flags);
int fs_write(int fd, void *buf, int size);
int fs_seek(int fd, long long offset, int whence);
int fs_read(int fd, void *buf, int size);
int fs_rename(const char *path, const char *name);
int fs_close(int fd);
int fs_delete(const char *path);
int fs_info(const char *path, struct fs_info_st *info);
int fs_info_child(struct fs_info_st *info, struct fs_info_st *child);
int fs_info_next(struct fs_info_st *info, struct fs_info_st *next);

#if defined (__cplusplus)
}
#endif

#endif