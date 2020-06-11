#include "device.h"
#include "path.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>

struct path_st *root;
long long nblock;
const char dev_path[] = "/dev/sdb";
int dev_fd = -1;


/* read entry to path */
void __load_entry_recur(path_t *p, path_t *parent, disk_ptr_t ptr)
{
    entry_t e;
    
    int err = dev_read(ptr, (void *)&e, sizeof(e));
    assert (err == sizeof(entry_t));
    strncpy(p->name, e.name, MAX_NAME_LEN);

    p->ptr = ptr;
    p->parent = parent;

    /* load next */
    if (e.next) {
        p->next = path_new();
        __load_entry_recur(p->next, parent, e.next);
    } else {
        p->next = NULL;
    }

    /* load child */
    if (e.type == ENTRY_TYPE_DIR && e.dir.sub_entry) {
        p->child = path_new();
        __load_entry_recur(p->child, p, e.dir.sub_entry);
    } else {
        p->child = NULL;
    }
}

/* do indexing */
int indexing()
{   
    root = path_new();
    root->next = NULL;
    __load_entry_recur(root, NULL, ROOT_PTR(nblock));
    return 0;
}

void __init_device()
{
    if (dev_fd >= 0) return;
    
    dev_fd = open(dev_path, O_RDWR);
    assert (dev_fd >= 0);

    super_block_t su;
    lseek(dev_fd, SUPER_PTR, SEEK_SET);
    read(dev_fd, (void *)&su, sizeof(su));
    nblock = su.nblock;

    // indexing 
    indexing();
}

int dev_write(disk_ptr_t ptr, void *buf, int size)
{
    __init_device();

    lseek(dev_fd, ptr, SEEK_SET);
    return write(dev_fd, buf, size);
}


int dev_read(disk_ptr_t ptr, void *buf, int size)
{
    __init_device();

    lseek(dev_fd, ptr, SEEK_SET);
    return read(dev_fd, buf, size);
}

path_t * getroot()
{
    __init_device();
    assert (root != NULL);
    return root;
}