#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include "../fs/fs_api.h"
#include "utils.h"
#include "fsmgr.h"

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

#define NEED_ARG(cmdline, n) \
    do {\
    if ((cmdline).size() < (n)) {\
        std::cout << "need " << (n) << " argument" << std::endl; \
        return -1;\
    }} while (0)

int FSMgr::help(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    int status = 0;
    if (cmdline.size() <= 1) {
        FSMgr::help({"help", "help"}, env);
        status = 0;
    } else {
        auto it = help_msg.find(cmdline[1]);
        if (it != help_msg.end())
            std::cout << it->second << std::endl;
        else 
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

int FSMgr::mkdir(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    int status = 0;
    NEED_ARG (cmdline, 2);

    std::string p = Utils::get_path(env["pwd"], cmdline[1]);

    if (check_dir_exist(p))
        status = 0;
    else 
        status = fs_create(p.c_str(), ENTRY_TYPE_DIR);
    return status;
}

int FSMgr::touch(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    int status = 0;
    NEED_ARG (cmdline, 2);

    std::string p = Utils::get_path(env["pwd"], cmdline[1]);

    if (check_file_exist(p)) 
        status = 0;
    else
        status = fs_create(p.c_str(), ENTRY_TYPE_FILE);
    return status;
}

int FSMgr::rm(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    NEED_ARG (cmdline, 2);

    std::string p = Utils::get_path(env["pwd"], cmdline[1]);

    int status = fs_delete(p.c_str());
    if (status)
        std::cout << p << " not found" << std::endl;
    return status;
}

int FSMgr::ls(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    int status = 0;
    fs_info_t info;
    std::string p;
    
    if (cmdline.size() <= 1) {
        p = Utils::get_path(env["pwd"], "");
    } else {
        p = Utils::get_path(env["pwd"], cmdline[1]);
    }

    status = fs_info(p.c_str(), &info);
    if (!status) {
        if (info.type == 'f') {
            Utils::pinfo(&info);
        } else if (info.type == 'd') {
            status = fs_info_child(&info, &info);
            if (!status) Utils::pinfo(&info);
            while (!status) {
                status = fs_info_next(&info, &info);
                if (!status) Utils::pinfo(&info);
            }
            status = 0;
        }
    } else {
        std::cout << p << " not found" << std::endl;
    }

    return status;
}

int FSMgr::cat(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    char ch = 0;
    int status = 0, err = 0;
    NEED_ARG (cmdline, 2);
    std::string p = Utils::get_path(env["pwd"], cmdline[1]);

    int fd = fs_open(p.c_str(), _O_RONLY);
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

int FSMgr::write(
        std::vector<std::string> cmdline, 
        std::map<std::string, std::string>& env)
{
    char ch = 0;
    int status = 0, err = 0;
    NEED_ARG (cmdline, 2);
    std::string p = Utils::get_path(env["pwd"], cmdline[1]);
    int fd = fs_open(p.c_str(), _O_WONLY);
    if (fd >= 0) {
        status = 0;
        while ((ch = getchar()) != EOF) {
            fs_write(fd, &ch, 1);
        }
        fs_close(fd);
        std::cin.clear();
    } else {
        status = -1;
    }

    return status;
}

int FSMgr::cd(
    std::vector<std::string> cmdline, 
    std::map<std::string, std::string>& env)
{
    NEED_ARG(cmdline, 2);
    int status = 0;

    std::string p = Utils::get_path(env["pwd"], cmdline[1]);
    if (check_dir_exist(p)) {
        env["pwd"] = p;
    } else {
        status = -1;
    }
    return status;
}

int FSMgr::_exit(
    std::vector<std::string> cmdline, 
    std::map<std::string, std::string>& env)
{
    exit(0);
    return 0;
}

int FSMgr::pwd(
    std::vector<std::string> cmdline, 
    std::map<std::string, std::string>& env)
{
    std::cout << Utils::get_path(env["pwd"], "") << std::endl;
    return 0;
}


void FSMgr::register_all_cmd(Cmd *cmd)
{
    cmd->register_cmd("help", FSMgr::help);
    cmd->register_cmd("mkdir", FSMgr::mkdir);
    cmd->register_cmd("touch", FSMgr::touch);
    cmd->register_cmd("rm", FSMgr::rm);
    cmd->register_cmd("ls", FSMgr::ls);
    cmd->register_cmd("cat", FSMgr::cat);
    cmd->register_cmd("write", FSMgr::write);
    cmd->register_cmd("cd", FSMgr::cd);
    cmd->register_cmd("exit", FSMgr::_exit);
    cmd->register_cmd("pwd", FSMgr::pwd);
}
