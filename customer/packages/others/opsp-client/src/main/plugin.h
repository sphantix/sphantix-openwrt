#ifndef __PLUGIN_H__
#define __PLUGIN_H__
#include <string>
#include <vector>
#include "plugin_info.h"

class CPlugin {
    private:
        int nTimeout;
        std::string sOpkgConfFile;

    private:
        void GetLocalPluginList(std::vector<CLocalPluginInfo> &vPluginList);
        bool SyncPluginList(const std::string &mac, const std::string &md5, const std::string &server_url, std::string &source_url, std::vector<CRetrunedPluginInfo> &vReturnedPluginInfo);
        bool Report(const std::string &report_url, const std::vector<CRetrunedPluginInfo> &vInfo);
        bool IsAlreadyInConf(const std::string &source_url);
        void UpdateOpkgConf(const std::string &source_url);

    public:
        CPlugin(): nTimeout(5), sOpkgConfFile("/etc/opkg.conf"){}
        void UpdatePlugins(const std::string &mac, const std::string &md5, const std::string &server_url);
};

#endif /* __PLUGIN_H__ */
