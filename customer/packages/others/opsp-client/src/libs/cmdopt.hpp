#ifndef __CMD_OPT_H__
#define __CMD_OPT_H__
#include <string>

class option{
    public:
        bool daemon;
        std::string config;
    public:
        option(): daemon(false), config("/etc/opsp-client.conf"){}
        int parser_cmd(int argc, char **argv);
};

#endif /* __CMD_OPT_H__ */
