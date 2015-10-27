#ifndef __CMD_OPT_H__
#define __CMD_OPT_H__
#include <string>

class option{
    public:
        bool daemon;
        std::string configfile;
    public:
        option(): daemon(false), configfile("/etc/opsp-client.conf"){}
        int parse_cmd(int argc, char **argv);
};

#endif /* __CMD_OPT_H__ */
