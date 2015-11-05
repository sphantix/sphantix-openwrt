#ifndef __HTTP_INTERFACE_H__
#define __HTTP_INTERFACE_H__

#include "plugin.h"
#include "sys_interface.h"

class CHttpInterface {
    public:
        CPlugin plugin;
        CSysInterface sysintf;
};

#endif /* __HTTP_INTERFACE_H__ */
