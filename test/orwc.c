#define DEBUG

#include "../fs/fs_api.h"
#include "../fs/path.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>

int main()
{
    int fd = fs_open("/tmp", _O_RONLY);
    printf("[open] /tmp %d\n", fd);

    fd = fs_open("/file/uestc.txt", _O_WONLY);
    printf("[open] /file/uestc.txt %d\n", fd);

    fs_write(fd, "test file read & write\n", 24);
    printf("[write] %d\n", fd);
    fs_close(fd);
    
    char buf[0x100] = { 0 };
    fd = fs_open("/file/uestc.txt", _O_RONLY);
    printf("[open] /file/uestc.txt %d\n", fd);
    fs_read(fd, buf, 24);
    printf("[read] %d\n", fd);
    puts(buf);

    fd = fs_open("/file/bigblock", _O_WONLY);
    printf("[open] /file/bigblock %d\n", fd);

    /* write big block */
    char x;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        x = (i % 10) + 0x30;
        fs_write(fd, &x, 1);
    }
    fs_close(fd);


    /* read big block */
    fd = fs_open("/file/bigblock", _O_RONLY);
    printf("[open] /file/bigblock %d\n", fd);
    for (int i = 0; i < BLOCK_SIZE; i++) {
        fs_read(fd, &x, 1);
        putc(x, stdout);
    }

    return 0;
}