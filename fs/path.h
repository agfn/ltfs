#ifndef __path_h__
#define __path_h__

#include "fs.h"

struct path_st {
    char name[MAX_NAME_LEN];
    disk_ptr_t ptr;
    struct path_st *parent; /*  */
    struct path_st *child;
    struct path_st *next;
};

typedef struct path_st path_t;

path_t * path_new();
void path_destroy(struct path_st *self);

void path_parse(struct path_st *self, const char *cpath);

int path_insert(struct path_st *self, struct path_st *p);
path_t * path_find(struct path_st *self, struct path_st *p);
int path_remove(struct path_st *self, struct path_st *p);
int path_rename(struct path_st *self, struct path_st *p, const char *name);

#endif