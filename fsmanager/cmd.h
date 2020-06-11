#ifndef __cmd_h__
#define __cmd_h__


#include "fsmgr.h"
#include <iostream>
#include <string>
#include <vector>

class Cmd {
private:
    FSMgr *mgr;
    std::string pwd;

public:
    Cmd();
    ~Cmd();
    void run();
    bool authentation();
    int exec(std::vector<std::string> cmdline);
};

#endif