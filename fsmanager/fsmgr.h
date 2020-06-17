/* 
 * File system manager.
 */

#ifndef __fsmgr_h__
#define __fsmgr_h__

#include <iostream>
#include <vector>
#include <map>
#include "cmd.h"

class FSMgr {

public:
    static bool check_dir_exist(std::string path);
    static bool check_file_exist(std::string file);
    static int help(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int mkdir(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int touch(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int rm(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int ls(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int cat(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    static int write(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    
    static int cd(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    
    static int _exit(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    
    static int pwd(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env);
    
    static void register_all_cmd(Cmd *cmd);
};

#endif
