#ifndef __utils_h__
#define __utils_h__

#include "../fs/fs.h"
#include "../fs/fs_api.h"

#if defined (__cplusplus)
extern "C" {
#endif

void phex(unsigned char *buf, int size);
void pentry(entry_t *entry);

void pinfo(fs_info_t *info);

#if defined (__cplusplus)
}
#endif

#endif