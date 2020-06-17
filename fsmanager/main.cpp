#include "fsmgr.h"
#include "cmd.h"

int main(int argc, char **argv)
{
    Cmd *cmd = new Cmd();
    FSMgr::register_all_cmd(cmd);
    cmd->run();

    return 0;
}