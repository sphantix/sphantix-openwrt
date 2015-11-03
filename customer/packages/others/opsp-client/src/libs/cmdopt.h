#ifndef __CMD_OPT_H__
#define __CMD_OPT_H__
#include <string>

class COption{
    public:
        bool bIsDaemon;
        std::string sConfigFile;
    public:
        COption(): bIsDaemon(false), sConfigFile("/etc/opsp-client.conf"){}
        int ParseCmd(int argc, char **argv);
};

#endif /* __CMD_OPT_H__ */
