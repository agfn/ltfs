#define DEBUG

#include "../fs/fs_api.h"
#include "../fs/path.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>

#define CREATE(type, path, itype) \
    do { \
        err = fs_create(path, itype); \
        printf("[create] [%c]%s %d\n", type, path, err);\
    } while (0)


char all_test_path[][0x100] = {
    "d/tmp",
    "f/tmp/a",
    "d/tmp/subdir",
    "f/tmp/subdir/a",
    "d/file",
    "f/file/uestc.txt",
    "f/file/bigblock",
    "d/del",
    "f/del/a",
    "d/del/subdir",
    "d/create",
    "f/create/fa",
    "f/create/fb",
    "f/create/fc",
    "d/create/da",
    "d/create/db",
    "d/create/dc"
};

int main()
{
    int err = 0;
    for (int i = 0; i < sizeof(all_test_path) / 0x100; i++) {
        if (all_test_path[i][0] == 'd')
            CREATE('d', &all_test_path[i][1], ENTRY_TYPE_DIR);
        else 
            CREATE('f', &all_test_path[i][1], ENTRY_TYPE_FILE);
    }

    return 0;
}