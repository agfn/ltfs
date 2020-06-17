#ifndef __utils_h__
#define __utils_h__

#include "../fs/fs.h"
#include "../fs/fs_api.h"
#include <string>

#if defined (__cplusplus)
extern "C" {
#endif

void phex(unsigned char *buf, int size);
void pentry(entry_t *entry);

#if defined (__cplusplus)
}
#endif

class Utils {
public:
    static std::string get_path(std::string pwd, std::string p);
    static void pinfo(fs_info_t *info);
    static std::string hex(unsigned char *buf, int size);
};

#endif