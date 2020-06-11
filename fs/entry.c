#include "entry.h"
#include "device.h"
#include "block.h"
#include "bitmap.h"
#include <string.h>

/* new entry table */
disk_ptr_t __new_entry_table()
{
    disk_ptr_t ph_table = block_new();
    entry_table_t new_et = {0};
    for (int i = 0 ; i < 0x100; i++)
        new_et.table[i] = block_new();
    dev_write(ph_table, (void *)&new_et, sizeof(entry_table_t));
    return ph_table;
}

disk_ptr_t entry_new()
{
    entry_table_t et;
    disk_ptr_t current;
    int idx;

    et.next = ENTRY_TABLE_PTR(nblock);

__try_next:
    while (et.next) {
        current = et.next;
        dev_read(current, (void *)&et, sizeof(entry_table_t));
        idx = __find_empty(et.entry_bmp, 0x100, 1);
        if (idx >= 0) {
            __set_inuse(et.entry_bmp, 0x100, idx, 1);
            dev_write(current, (void *)&et, sizeof(entry_table_t));
            return et.table[idx / 32] 
                + sizeof(entry_t) * (idx % 32);
        }
    }
    
    /* create entry table */
    disk_ptr_t ph_table = __new_entry_table();
    et.next = ph_table;
    goto __try_next;
}

int entry_destroy(disk_ptr_t ptr)
{
    entry_table_t et;
    disk_ptr_t current;
    entry_t e;
    int idx;
    disk_ptr_t block_ptr = ~BLOCK_MASK & ptr, 
        inblock_ptr = ptr - block_ptr;

    et.next = ENTRY_TABLE_PTR(nblock);

    while (et.next) {
        current = et.next;
        dev_read(current, (void *)&et, sizeof(entry_table_t));
        for (int i = 0; i < 0x100; i++) {
            if (et.table[i] == block_ptr) {
                int j = inblock_ptr / sizeof(entry_t);
                __set_empty(et.entry_bmp, 0x100, i * 32 + j, 1);
                dev_write(current, (void *)&et, sizeof(entry_table_t));
                
                dev_read(ptr, (void *)&e, sizeof(e));
                if (e.type == ENTRY_TYPE_DIR) {
                    e.next = e.dir.sub_entry;
                    while (e.next) {
                        disk_ptr_t current = e.next; 
                        dev_read(current, (void *)&e, sizeof(e));
                        entry_destroy(current);
                    }
                }
                return 0;
            }
        }
    }

    return -1;
}

int entry_insert(disk_ptr_t parent, disk_ptr_t child)
{
    entry_t eparent, echild = { 0 };
    dev_read(parent, (void *)&eparent, sizeof(eparent));
    if (eparent.type != ENTRY_TYPE_DIR) return -1;

    // dev_read(child, (void *)&echild, sizeof(echild));

    echild.next = eparent.dir.sub_entry;
    eparent.dir.sub_entry = child;
    dev_write(parent, (void *)&eparent, sizeof(eparent));
    dev_write(child, (void *)&echild, sizeof(echild));
    return 0;
}

int entry_rm_child(disk_ptr_t parent, disk_ptr_t child_next)
{
    int err = -1;
    entry_t eparent;
    dev_read(parent, (void *)&eparent, sizeof(eparent));
    if (eparent.type != ENTRY_TYPE_DIR) return -1;

    eparent.dir.sub_entry = child_next;
    dev_write(parent, (void *)&eparent, sizeof(eparent));
    
    return 0;
}

int entry_rm_next(disk_ptr_t prev, disk_ptr_t next_next)
{
    int err = -1;
    entry_t ep;
    dev_read(prev, (void *)&ep, sizeof(ep));

    ep.dir.sub_entry = next_next;
    dev_write(prev, (void *)&ep, sizeof(ep));
    
    return 0;
}

void entry_rename(disk_ptr_t ptr, const char *name)
{
    entry_t e;
    dev_read(ptr, (void *)&e, sizeof(e));
    strncpy(e.name, name, MAX_NAME_LEN);
    dev_write(ptr, (void *)&e, sizeof(e));
}

