#ifndef __HTTP_INTERFACE_H__
#define __HTTP_INTERFACE_H__

#include <string>
#include "plugin.h"
#include "sys_interface.h"

class CHttpInterface {
    public:
        CPlugin plugin;
        CSysInterface sysintf;

    public:
        void Initialize(const std::string &server_url, const std::string &mac, const std::string &file);
};

#endif /* __HTTP_INTERFACE_H__ */
