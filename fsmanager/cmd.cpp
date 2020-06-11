#include "cmd.h"
#include <openssl/md5.h>
#include <boost/program_options.hpp>
#include <vector>
#include <sstream>
#include <iterator>
#include <string>
#include <map>


std::string CEND = "\033[0m";
/*
You can use these ANSI escape codes:
Black        0;30     Dark Gray     1;30
Red          0;31     Light Red     1;31
Green        0;32     Light Green   1;32
Brown/Orange 0;33     Yellow        1;33
Blue         0;34     Light Blue    1;34
Purple       0;35     Light Purple  1;35
Cyan         0;36     Light Cyan    1;36
Light Gray   0;37     White         1;37 
*/
std::map<std::string, std::string> colortable = {
    {"red", "\033[0;31m"},
    {"green", "\033[0;32m"},
    {"blue", "\033[0;34m"}
};

Cmd::Cmd()
{
    this->pwd = std::string("/");
    this->mgr = new FSMgr();
}

Cmd::~Cmd()
{
    delete this->mgr;
}

void Cmd::run() 
{
    /* do authentation */
    if (!this->authentation()) {
        return;
    }

    this->mgr->help("help");
    
    int status = 0;
    std::string cmdline = std::string();

    while (1) {
        std::string statuscolor;
        if (status) statuscolor += colortable["red"];
        else statuscolor += colortable["green"];
        std::cout 
            << statuscolor << status << CEND 
            << " " 
            << colortable["blue"] << this->pwd << CEND
            << " > ";
    
        if (std::getline(std::cin, cmdline));
        std::stringstream ss(cmdline);
        std::vector<std::string> cmdvect;
        std::string item;
        while (ss >> item) {
            cmdvect.push_back(item);
        }
        status = this->exec(cmdvect);
    }
}


std::string hex(unsigned char *buf, int length)
{
    std::string s;
    char hextable[] = "0123456789ABCDEF";
    for (int i = 0; i < length; i++) {
        s += hextable[(buf[i] >> 4) & 0xf];
        s += hextable[buf[i] & 0xf];
    }
    return s;
}

bool Cmd::authentation()
{
    bool pass = false;
    static int try_count = 0;
    std::string password;
    unsigned char buf[0x100] = { 0 };
    std::string right_hash = "2629FF40AA6E7E4C8F9B75DE6B616FBC";

    password = std::string(getpass("password: "));

    MD5((unsigned char *)password.c_str(), password.length(), buf);
    if (right_hash == hex(buf, 16)) {
        pass = true;
    } else {
        try_count++;
        if (try_count >= 3) 
            return false;
        
        std::cout << "wrong, try again!" << std::endl;
        pass = this->authentation();
    }

    return pass;
}

std::string path_join(std::string parent, std::string child)
{
    if (child[0] == '/') child.erase(0);
    if (parent.back() == '/') parent.pop_back();
    return parent + "/" + child;
}

int Cmd::exec(std::vector<std::string> cmdline)
{
    int  status = 0;
    std::string fullpath;
    if (cmdline.size() < 1) return 0;

    std::string cmd = cmdline[0];

    if (cmdline.size() < 2) {
        if (cmd == "pwd") {
            std::cout << this->pwd << std::endl;
            status = 0;
        } else if (cmd == "exit") {
            exit(0);
        } else if (cmd == "ls") {
            status = this->mgr->ls(this->pwd);
        } else if (cmd == "help") {
            status = this->mgr->help("help");
        } else {
            goto __cmd_not_found;
        }
    } else {
        if (cmd == "help") {
            status = this->mgr->help(cmdline[1]);
            return status;
        }

        if (cmdline[1][0] != '/') {
            fullpath = path_join(this->pwd, cmdline[1]);
        } else {
            fullpath = cmdline[1];
        }

        if (cmd == "cd") {
            if (this->mgr->check_dir_exist(fullpath)) {
                this->pwd = fullpath;
                status = 0;
            } else {
                status = -1;
            }
        } else if (cmd == "mkdir") {
            status = this->mgr->mkdir(fullpath);
        } else if (cmd == "touch") {
            status = this->mgr->touch(fullpath);
        } else if (cmd == "rm") {
            status = this->mgr->rm(fullpath);
        } else if (cmd == "ls") {
            status = this->mgr->ls(fullpath);
        } else if (cmd == "cat") {
            status = this->mgr->cat(fullpath);
        } else if (cmd =="write") {
            status = this->mgr->write(fullpath);
        } else {
__cmd_not_found:
            std::cout 
                << colortable["red"] 
                << cmd << " not found" 
                << CEND
                << std::endl;
            status = -1;
        }
    }

    return status;
}