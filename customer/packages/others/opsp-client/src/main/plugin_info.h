#ifndef __PLUGIN_INFO_H__
#define __PLUGIN_INFO_H__
#include <string>

const int ACTION_REMOVE     =   -1;
const int ACTION_INSTALL    =   1;
const int ACTION_NONE       =   0;

class CLocalPluginInfo {
    public:
        std::string sName; 
        std::string sVersion;
        
    public:
        CLocalPluginInfo(): sName(""), sVersion(""){}
        bool operator == (const CLocalPluginInfo &plugin) {return (this->sName == plugin.sName);}
};

class CRetrunedPluginInfo {
    private:
        std::string sDownloadFileName;
        std::string sLocalSavePath;

    public:
        int nId;
        int nAction;
        int nOperationResult;
        std::string sName;
        std::string sVersion;
        std::string sUrl;
        std::string sOperationLog;

    private:
        void GetDownloadFileName(void);
        bool CheckPluginUpdated(void);
        bool CheckPluginExist(void);
        void DoCleanUp(void);
        bool DoOpkgCmd(const std::string &cmd);
        
    public:
        CRetrunedPluginInfo(): sLocalSavePath("/var/"), nId(0), nAction(0), nOperationResult(0), sName(""), sVersion(""), sUrl(""), sOperationLog(""){}
        void Install(void);
        void Remove(void);
        void ReInstall(void);
};
#endif /* __PLUGIN_INFO_H__ */
