#include "fsmgr.h"
#include <iostream>
#include <string>
#include <map>
#include "../fs/fs_api.h"
#include "utils.h"

std::map<std::string, std::string> help_msg = {
    {"help", "help [cmd]{cd, mkdir, touch, rm, pwd, ls, cat, write}"},
    {"cd", "cd [path]"},
    {"mkdir", "mkdir [path]"},
    {"touch", "touch [file]"},
    {"rm", "rm [path]"},
    {"pwd", "pwd -> current path"},
    {"ls", "ls [path/file]"},
    {"cat", "cat [file]"},
    {"write", "write [file]"}
};

int FSMgr::help(std::string cmd) 
{
    int status = 0;
    if (help_msg.count(cmd)) {
        std::cout << help_msg[cmd] << std::endl;
    } else {
        status = -1;
    }

    return status;
}

bool FSMgr::check_dir_exist(std::string path)
{
    fs_info_t info;
    if (!fs_info(path.c_str(), &info)) {
        if (info.type == 'd') 
            return true;
    }
    return false;
}

bool FSMgr::check_file_exist(std::string file)
{
    fs_info_t info;
    if (!fs_info(file.c_str(), &info)) {
        if (info.type == 'f') 
            return true;
    }
    return false;
}

int FSMgr::mkdir(std::string path)
{
    int status = 0;
    if (check_dir_exist(path))
        status = 0;
    else 
        status = fs_create(path.c_str(), ENTRY_TYPE_DIR);
    return status;
}

int FSMgr::touch(std::string file)
{
    int status = 0;
    if (check_file_exist(file)) 
        status = 0;
    else
        status = fs_create(file.c_str(), ENTRY_TYPE_FILE);
    return status;
}

int FSMgr::rm(std::string path)
{
    int status = fs_delete(path.c_str());
    if (status)
        std::cout << path << " not found" << std::endl;
    return status;
}

int FSMgr::ls(std::string path)
{
    int status = 0;
    fs_info_t info;
    status = fs_info(path.c_str(), &info);
    if (!status) {
        if (info.type == 'f') {
            pinfo(&info);
        } else if (info.type == 'd') {
            status = fs_info_child(&info, &info);
            if (!status) pinfo(&info);
            while (!status) {
                status = fs_info_next(&info, &info);
                if (!status) pinfo(&info);
            }
            status = 0;
        }
    } else {
        std::cout << path << " not found" << std::endl;
    }

    return status;
}

int FSMgr::cat(std::string file)
{
    char ch = 0;
    int status = 0, err = 0;
    int fd = fs_open(file.c_str(), _O_RONLY);
    if (fd >= 0) {
        status = 0;
        while ((err = fs_read(fd, (void *)&ch, 1)) > 0) {
            std::cout << ch;
        }
        std::cout << std::endl;
        fs_close(fd);
    } else {
        status = -1;
    }
    return status;
}

int FSMgr::write(std::string file)
{
    char ch = 0;
    int status = 0, err = 0;
    int fd = fs_open(file.c_str(), _O_WONLY);
    if (fd >= 0) {
        status = 0;
        while (std::cin >> ch) {
            fs_write(fd, &ch, 1);
        }
        fs_close(fd);
        std::cin.clear();
    } else {
        status = -1;
    }

    return status;
}