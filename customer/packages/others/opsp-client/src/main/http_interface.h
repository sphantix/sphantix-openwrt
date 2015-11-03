#ifndef __HTTP_INTERFACE__
#define __HTTP_INTERFACE__
#include "plugin.h"
#include "sys_interface.h"

class CHttpInterface {
    public:
        CPlugin plugin;
        CSysInterface sysintf;
};

#endif /* __HTTP_INTERFACE__ */
