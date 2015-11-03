#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include "client.h"
#include "ws_thread.h"
#include "handler_thread.h"

CClient client; 

volatile bool CClient::bKeepLooping = true;

int main(int argc, char **argv)
{
    //parse command
    client.option.ParseCmd(argc, argv);

    if (client.option.bIsDaemon)
    {
        switch (client.Fork())
        {
            case 0:                /* child */
                setsid();
                client.Run();
                break;

            default:               /* parent */
                exit(E_SUCCESS);
                break;
        }
    }
    else
    {
        client.Run();
    }

    return 0;
}
