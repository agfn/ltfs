#include "utils.h"
#include <stdio.h>

void phex(unsigned char *buf, int size)
{
    for (int i = 0; i < size; i++) {
        printf("%02x ", buf[i]);
    }

    puts("");
}


void pentry(entry_t *entry)
{
    printf("%s\n"
        "\ttype = %x\n"
        "\tpermission = %x %x %x\n"
        "\tflags = %x\n"
        "\tnext = %llx\n", 
        entry->name, 
        entry->type,
        entry->permission[0], entry->permission[1], entry->permission[2], 
        entry->flags,
        entry->next);
    if (entry->type == ENTRY_TYPE_FILE) {
        printf(
            "\t\tptr = %llx\n"
            "\t\tsize = %llx\n",
            entry->file.ptr,
            entry->file.size);
    } else if (entry->type == ENTRY_TYPE_DIR) {
        printf("\t\tsub_entry = %llx\n", entry->dir.sub_entry);
    }
}

void pinfo(fs_info_t *info)
{
    info->reserve[0] = 0;
    printf("[%c]%s 0x%llx %lld %s\n",
        info->type,
        info->permission,
        info->entry_ptr,
        info->size,
        info->name);
}