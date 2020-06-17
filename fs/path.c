#include "path.h"
#include "entry.h"
#include <stdlib.h>
#include <string.h>

path_t * path_new()
{
    return (path_t *)malloc(sizeof(path_t));
}

void path_destroy(struct path_st *self)
{
    path_t *p = self, *tofree;
    while (p) {
        free(p);
        p = p->child;
    }
}


void path_parse(struct path_st *self, const char *cpath)
{
    int length = strlen(cpath);
    int find_new = 0;
    char *vpath = (char *)malloc(length + 1);
    strcpy(vpath, cpath);

    strcpy(self->name, "/");
    self->next = NULL;
    self->parent = NULL;
    self->ptr = 0;
    path_t *p = self;

    for (int i = 0; i < length; i++) {
        if (vpath[i] == '/')
            vpath[i] = 0;
    }

    for (int i = 0; i < length;) {
        if (vpath[i]) {
            p->child = path_new();
            strncpy(p->child->name, &vpath[i], MAX_NAME_LEN);
            p->child->parent = p;
            p->child->next = NULL;
            p->child->ptr = 0;
            p = p->child;
            i += strlen(&vpath[i]);
        } else i++;
    }
    if (p) p->child = NULL;
    self->parent = p;       // last child

    free(vpath);
}


int path_insert(struct path_st *self, struct path_st *path)
{
    path_t *last_child = path->parent, *new_child;
    int err = 0;

    // already exist
    if (path_find(self, path)) return -1;

    new_child = last_child->parent->child;   // save value
    last_child->parent->child = NULL;
    path_t *parent = path_find(self, path);
    last_child->parent->child = new_child;

    if (parent) {
        new_child = path_new();
        strncpy(new_child->name, last_child->name, MAX_NAME_LEN);
        new_child->child = NULL;
        new_child->next = parent->child;
        new_child->parent = parent;
        parent->child = new_child;
        new_child->ptr = entry_new();
        err = entry_insert(parent->ptr, new_child->ptr);
        entry_rename(new_child->ptr, new_child->name);
    } else err = -1;

    return err;
}

path_t * path_find(struct path_st *self, struct path_st *path)
{
    path_t *p = self;
    while (p) {
        if (!strncmp(path->name, p->name, MAX_NAME_LEN)) {
            if (path->child) {
                p = p->child;
                path = path->child;
                continue;
            } else {
                return p;
            }
        }
        p = p->next;
    }

    return NULL;
}

int path_remove(struct path_st *self, struct path_st *path)
{
    int err = -1;
    path_t *p = path_find(self, path);
    disk_ptr_t next = 0;
    if (p && p->next) next = p->next->ptr;

    if (p && p->parent) {
        if (p->parent->child == p) {
            // direct child
            p->parent->child = p->next;
            err = entry_rm_child(p->parent->ptr, next);
        } else {
            // find in the middle 
            path_t *ptmp = p->parent->child;
            while (ptmp && ptmp->next) {
                if (ptmp->next == p) {
                    ptmp->next = p->next;
                    err = entry_rm_next(ptmp->ptr, next);
                    break;
                }
                ptmp = ptmp->next;
            }
        }

        if (!err) {
            err = entry_destroy(p->ptr);
            path_destroy(p);
        }
    } else err = -1;

    return err;
}

int path_rename(struct path_st *self, struct path_st *path, const char *name)
{
    path_t *p = path_find(self, path);
    if (p) {
        strncpy(p->name, name, MAX_NAME_LEN);
        entry_rename(p->ptr, name);
        return 0;
    }

    return -1;
}

