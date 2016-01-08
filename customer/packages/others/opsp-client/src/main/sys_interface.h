#ifndef __SYS_INTERFACE_H__
#define __SYS_INTERFACE_H__
#include <string>
#include <vector>

class CSysInterface {
    public:
        void Reset(void);
        void Reboot(void);
        void ReboundShell(const std::string &mac);
        void ReboundTTY(const std::string &mac);
};

#endif/* __SYS_INTERFACE_H__ */
