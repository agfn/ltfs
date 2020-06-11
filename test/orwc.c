#define DEBUG

#include "../fs/fs_api.h"
#include "../fs/path.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>

int main()
{
    int fd = fs_open("/tmp", 0);
    printf("[open] /tmp %d\n", fd);

    fd = fs_open("/tmp/test", 0);
    printf("[open] /tmp/test %d\n", fd);
    fs_close(fd);

    fd = fs_open("/test", 0);
    printf("[open] /test %d\n", fd);

    fd = fs_open("/file/uestc.txt", 0);
    printf("[open] /file/uestc.txt %d\n", fd);

    fs_write(fd, "test file read & write\n", 24);
    printf("[write] %d\n", fd);
    
    char buf[0x100] = { 0 };
    fs_seek(fd, 0, _SEEK_SET);
    fs_read(fd, buf, 24);
    printf("[read] %d\n", fd);
    puts(buf);

    fd = fs_open("/file/bigblock", 0);
    printf("[open] /file/bigblock %d\n", fd);

    /* write big block */
    char x;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        x = (i % 10) + 0x30;
        fs_write(fd, &x, 1);
    }

    /* read big block */
    fs_seek(fd, 0, _SEEK_SET);
    for (int i = 0; i < BLOCK_SIZE; i++) {
        fs_read(fd, &x, 1);
        putc(x, stdout);
    }

    return 0;
}