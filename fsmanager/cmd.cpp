#include <openssl/md5.h>
#include <boost/program_options.hpp>
#include <vector>
#include <sstream>
#include <iterator>
#include <string>
#include <map>
#include "cmd.h"
#include "utils.h"


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
    this->env = {{"pwd", "/"}};
}

Cmd::~Cmd()
{
}

void Cmd::run() 
{
    /* do authentation */
    if (!this->authentation()) {
        return;
    }

    this->exec({"help"});
    
    int status = 0;
    std::string cmdline = std::string();

    while (1) {
        std::string statuscolor;
        if (status) statuscolor += colortable["red"];
        else statuscolor += colortable["green"];
        std::cout 
            << statuscolor << status << CEND 
            << " " 
            << colortable["blue"] << this->env["pwd"] << CEND
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


bool Cmd::authentation()
{
    bool pass = false;
    static int try_count = 0;
    std::string password;
    unsigned char buf[0x100] = { 0 };
    std::string right_hash = "2629FF40AA6E7E4C8F9B75DE6B616FBC";

    password = std::string(getpass("password: "));

    MD5((unsigned char *)password.c_str(), password.length(), buf);
    if (right_hash == Utils::hex(buf, 16)) {
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

    auto it = this->cmd_list.find(cmdline[0]);
    if (it != this->cmd_list.end()) {
        status = it->second(cmdline, this->env);
    } else {
        status = -1;
        std::cout << colortable["red"]
            << cmdline[0] << " not found" 
            << CEND << std::endl;
    }

    return status;
}


void Cmd::register_cmd(std::string name, ICmdFunc cmd)
{
    this->cmd_list[name] = cmd;
}