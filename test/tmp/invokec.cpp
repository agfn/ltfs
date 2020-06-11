#include <iostream>
#include "../../fs/fs_api.h"

int main()
{
    fs_info_t info;
    fs_info("/tmp", &info);
    
    return 0;
}