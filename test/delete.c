#define DEBUG

#include "../fs/fs_api.h"
#include "../fs/path.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>

#define DEL(type, path) \
    do { \
        err = fs_delete(path); \
        printf("[delete] [%c]%s %d\n", type, path, err);\
    } while (0)


int main()
{
    int err = 0;
    DEL('f', "/del/a");
    DEL('d', "/del/subdir");

    return 0;
}