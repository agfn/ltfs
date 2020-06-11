#include "../fs/fs_api.h"
#include <stdio.h>
#include "utils.h"

int main()
{
    fs_info_t info = { 0 };

    int err = fs_info("/file/bigblock", &info);
    printf("[info] [f]/file/bigblock %d\n", err);
    pinfo(&info);

    while (!err) {
        err = fs_info_next(&info, &info);
        printf("[info] [f]/file/%s %d\n", info.name, err);
        if (!err) pinfo(&info);
    }

    return 0;
}