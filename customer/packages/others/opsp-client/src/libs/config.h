#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <string>
#include <libsol-util/utl.h>

class CConfig{
    public:
        int nLogLevel;
        std::string sLogDest;
        std::string sLogPath;
        std::string sLogName;
        std::string sLogFile;
    private:
        static UBOOL8 HandleConfig(const char *group, const char *name, const char *value, void *cxdata);
        void ParseConfig(const std::string &configfile);
        void CheckConfig(void);
    public:
        CConfig(): nLogLevel(0) {}
        void InitConfig(const std::string &configfile);
};

#endif /* __CONFIG_H__ */
