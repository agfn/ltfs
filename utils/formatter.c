#include "../fs/fs.h"
#include "../fs/block.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>


int main(int argc, char **argv)
{
    assert (argc >= 2);

    super_block_t super = { 0 };
    uint_t *block_bmp, bmp_size;
    entry_table_t et = { 0 };
    entry_t root = { 0 };

    
    int fd = open(argv[1], O_WRONLY);
    assert (fd >= 0);
    
    /* get disk size */
    lseek(fd, 0, SEEK_SET);
    long long dsize = lseek(fd, 0, SEEK_END);
    printf("size: %llx\n", dsize);
    printf("super block: %llx\n", (long long)SUPER_PTR);
    super.nblock = dsize / BLOCK_SIZE;
    bmp_size = (super.nblock + 31) / 32 * 4;

    block_bmp = (uint_t *)calloc(bmp_size, 1);
    
    int N = N_BMP_BLOCK(super.nblock) + 1 + 0x100;
    for (int i = 0; i < N / 32; i++) {
        block_bmp[i] = 0xffffffff;
    }
    block_bmp[N / 32] = (1 << (N % 32 + 1)) - 1;
    block_bmp[super.nblock / 32] = ~((1 << (super.nblock % 32 + 1)) - 1);

    printf("entry table: %llx\n", (long long)ENTRY_TABLE_PTR(super.nblock));

    disk_ptr_t ptr = ROOT_PTR(super.nblock);
    printf("root: %llx", ptr);
    for (int i = 0; i < 0x100; i++) {
        et.table[i] = ptr;
        ptr += BLOCK_SIZE;
    }
    et.entry_bmp[0] = 1;    // root

    root.type = ENTRY_TYPE_DIR;
    root.name[0] = '/';

    /* write to block */
    lseek(fd, SUPER_PTR, SEEK_SET);
    write(fd, &super, sizeof(super_block_t));
    write(fd, block_bmp, bmp_size);

    lseek(fd, ENTRY_TABLE_PTR(super.nblock), SEEK_SET);
    write(fd, &et, sizeof(entry_table_t));
    
    lseek(fd, ROOT_PTR(super.nblock), SEEK_SET);
    write(fd, &root, sizeof(entry_t));

    close(fd);

    return 0;
}