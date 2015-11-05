#ifndef __SYS_INTERFACE_H__
#define __SYS_INTERFACE_H__
#include <string>
#include <vector>

class CSysInterface {
    private:
        void DoTruelyReboundShell(const std::string &mac);
    public:
        void Reboot(void);
        void ReboundShell(const std::string &mac);
};

#endif/* __SYS_INTERFACE_H__ */
