/* 
 * Describe file system structure and access API.
 */

#ifndef __fs_h__
#define __fs_h__

#include "types.h"

/* *** disk related structure *** */


#define BLOCK_SIZE  4096            /* 4K / block */
#define BLOCK_MASK  0xfff
typedef unsigned int block_no_t;    /* block number (spport 16T) */
typedef long long disk_ptr_t;

#define ENTRY_TABLE_SIZE    0x10000
#define MAX_NAME_LEN        0x60
#define SUPER_PTR           0x200

typedef struct __super_block_st
{
    uint_t nblock;   // has n block 
} super_block_t;

#define N_BMP_BLOCK(nblock)         (\
    (\
    (((nblock) + 31) / 32) * 4 \
    + sizeof(super_block_t) \
    + BLOCK_SIZE - 1\
    ) / BLOCK_SIZE)

#define ENTRY_TABLE_PTR(nblock)     (N_BMP_BLOCK(nblock) * BLOCK_SIZE)


typedef struct __entry_table_st {
    disk_ptr_t table[0x100];
    uint_t entry_bmp[0x100];    // 32 * 0x100 
    disk_ptr_t next;
} entry_table_t;

#define ROOT_PTR(nblock)    (ENTRY_TABLE_PTR(nblock) + BLOCK_SIZE)


typedef struct __internal_file_desc_st
{
    disk_ptr_t ptr;
    ull_t size;
} __internal_file_desc_t;

typedef struct __internal_dir_desc_st
{
    disk_ptr_t sub_entry;
} __internal_dir_desc_t;


#define ENTRY_TYPE_DIR  0
#define ENTRY_TYPE_FILE 1

#define ENTRY_FLAG_INUSE    1

typedef struct __entry_st
{
    char name[MAX_NAME_LEN];
    char type;
    char permission[3];
    char flags;
    char reserve[3];
    union 
    {
        __internal_file_desc_t file;
        __internal_dir_desc_t dir;
    };
    disk_ptr_t next;
} entry_t;

#endif
