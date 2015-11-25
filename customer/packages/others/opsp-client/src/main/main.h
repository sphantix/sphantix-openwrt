#ifndef __MAIN_H__
#define __MAIN_H__
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

const int E_SUCCESS                 = 0;
const int E_FORKERROR               = -1;
const int E_NOCONFIGFILE            = -2;
const int E_CONFIGCHECKERR          = -3;
const int E_THREADWSERR             = -4;
const int E_THREADHANDLERERR        = -5;
const int E_SETSIGNALERR            = -6;
const int E_POPENERROR              = -7;
const int E_DOWNLOADERR             = -8;
//ipk opreation
const int E_IPK_INSTALLERR          = -1001;
const int E_IPK_REMOVEERR           = -1002;

const int E_OPKG_CMDERR             = -2001;

const std::string OPSP_CLIENT_VERSION("1.0.0");
const std::string OPSP_SERVER_CLUSTER("opsp-s1.hi-wifi.cn;opsp-s2.hi-wifi.cn;opsp-s3.hi-wifi.cn;opsp-s4.hi-wifi.cn;opsp-s5.hi-wifi.cn");

//use to get correct return value
typedef void (*sighandler_t)(int);  
inline int safe_system(const char *cmd_line)  
{  
    int ret = 0;  
    sighandler_t old_handler;

    old_handler = signal(SIGCHLD, SIG_DFL);
    ret = system(cmd_line);
    signal(SIGCHLD, old_handler);
    
    if(ret == -1)
    {
        return -1;
    }

    if(WIFEXITED(ret))
    {
        return WEXITSTATUS(ret);
    }
    else
    {
        return WEXITSTATUS(ret);
    }
    
    return ret;  
}
#endif /* __MAIN_H__ */
