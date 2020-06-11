
#include "cmd.h"

int main(int argc, char **argv)
{
    Cmd *cmd = new Cmd();
    cmd->run();

    return 0;
}