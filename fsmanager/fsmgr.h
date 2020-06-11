/* 
 * File system manager.
 */

#ifndef __fsmgr_h__
#define __fsmgr_h__

#include <iostream>

class FSMgr {

public:
    bool check_dir_exist(std::string path);
    bool check_file_exist(std::string file);
    int help(std::string cmd);
    int mkdir(std::string path);
    int touch(std::string file);
    int rm(std::string path);
    int ls(std::string path);
    int cat(std::string file);
    int write(std::string file);
};

#endif
