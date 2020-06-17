#ifndef __cmd_h__
#define __cmd_h__

#include <iostream>
#include <string>
#include <vector>
#include <map>

typedef int (*ICmdFunc)(std::vector<std::string>, std::map<std::string, std::string>&);

class Cmd {
private:
    std::map<std::string, ICmdFunc> cmd_list;
    std::map<std::string, std::string> env;

public:
    Cmd();
    ~Cmd();
    void run();
    bool authentation();
    int exec(std::vector<std::string> cmdline);
    void register_cmd(std::string name, ICmdFunc cmd);
};

#endif