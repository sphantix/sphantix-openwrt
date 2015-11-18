#include <cstdio>
#include <string>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <curl/curl.h>
#include <libsol-util/utl_logging.h>
#include "plugin.h"
#include "plugin_info.h"
#include "cJSON.h"
#include "restclient.h"

void CPlugin::GetLocalPluginList(std::vector<CLocalPluginInfo> &vPluginList)
{
    FILE *fp = NULL;
    char buff[128] = {'\0'};
    char name[32] = {'\0'};
    char version[128] = {'\0'};
    std::string cmd("opkg list");

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        while(fgets(buff, sizeof(buff), fp) != NULL)
        {
            if (strstr(buff, "hiwifi") != NULL) 
            {
                CLocalPluginInfo plugin;
                sscanf(buff, "%s - %s\n", name, version);
                plugin.sName = name;
                plugin.sVersion = version;
                utlLog_debug("name = %s, version = %s", plugin.sName.c_str(), plugin.sVersion.c_str());

                auto it = std::find(vPluginList.begin(), vPluginList.end(), plugin);
                if(it != vPluginList.end())
                {
                    if (it->sVersion != plugin.sVersion)
                        it->sVersion = plugin.sVersion; 
                }
                else
                {
                    vPluginList.push_back(plugin);
                }
            }
        }
        pclose(fp);
    }
    else
    {
        utlLog_error("popen error, exit!");
    }
}

bool CPlugin::SyncPluginList(const std::string &mac, const std::string &md5, const std::string &server_url, 
        std::string &source_url, std::vector<CRetrunedPluginInfo> &vReturnedPluginInfo)
{
    //get local hiwifi opkg list
    std::vector<CLocalPluginInfo> local_plugin_list;
    GetLocalPluginList(local_plugin_list);

    //combine json data
    cJSON *pRoot = cJSON_CreateObject();
    cJSON *pMac = cJSON_CreateString(mac.c_str());
    cJSON_AddItemToObject(pRoot, "mac", pMac);
    cJSON *pMD5 = cJSON_CreateString(md5.c_str());
    cJSON_AddItemToObject(pRoot, "kernelMD5", pMD5);

    cJSON *pPlugins = cJSON_CreateArray();

    for(auto it = local_plugin_list.begin(); it != local_plugin_list.end(); ++it)
    {
        cJSON *pPlugin = cJSON_CreateObject();
        cJSON *pName = cJSON_CreateString(it->sName.c_str());
        cJSON *pVersion = cJSON_CreateString(it->sVersion.c_str());

        cJSON_AddItemToObject(pPlugin, "name", pName);
        cJSON_AddItemToObject(pPlugin, "version", pVersion);
        cJSON_AddItemToArray(pPlugins, pPlugin);
    }

    cJSON_AddItemToObject(pRoot, "plugins", pPlugins);
    char *szPost = cJSON_Print(pRoot);

    utlLog_debug("buff = %s", szPost);

    utlLog_debug("server_url = %s/ap/info/plugin/list", server_url.c_str());

    //send to http server
    RestClient::response r = RestClient::post(server_url + "/ap/info/plugin/list", "application/json" , std::string(szPost), nTimeout);
    cJSON_Delete(pRoot);
    pRoot = NULL;
    free(szPost);

    //judge return code
    if (r.code != 200)
        return false;

    utlLog_debug("return data = %s", r.body.c_str());
    // got return data
    pRoot = cJSON_Parse(r.body.c_str());
    if (pRoot == NULL)
        return false;

    // server error
    cJSON *pResult = cJSON_GetObjectItem(pRoot, "result");
    if (pResult)
    {
        cJSON_Delete(pRoot);
        return false;
    }

    cJSON *pSourceUrl = cJSON_GetObjectItem(pRoot, "source_url");
    if (pSourceUrl != NULL) 
        source_url = pSourceUrl->valuestring; 

    cJSON *pPluginList = cJSON_GetObjectItem(pRoot, "plugins");
    if(pPluginList != NULL){
        int size = cJSON_GetArraySize(pPluginList);

        for(int i = 0; i < size; ++i)
        {
            cJSON *pele = cJSON_GetArrayItem(pPluginList, i);
            cJSON *pName = cJSON_GetObjectItem(pele, "name");
            cJSON *pId = cJSON_GetObjectItem(pele, "plugins_id");
            cJSON *pInstall = cJSON_GetObjectItem(pele, "reinstall");
            cJSON *pVersion = cJSON_GetObjectItem(pele, "version");
            cJSON *pUrl = cJSON_GetObjectItem(pele, "url");

            if( pName == NULL || pId == NULL || pInstall == NULL || pVersion == NULL || pUrl == NULL)
                continue;
            else
            {
                CRetrunedPluginInfo plugin_info;
                plugin_info.sName = pName->valuestring;
                plugin_info.sVersion = pVersion->valuestring;
                plugin_info.sUrl = pUrl->valuestring;
                plugin_info.nId = pId->valueint;
                plugin_info.nAction = pInstall->valueint;

                vReturnedPluginInfo.push_back(plugin_info);
            }
        }
    }
    cJSON_Delete(pRoot);

    return true;
}

bool CPlugin::Report(const std::string &report_url, const std::vector<CRetrunedPluginInfo> &vInfo)
{
    cJSON *pRoot = cJSON_CreateObject();
    if (pRoot == NULL)
    {
        return false;
    }

    cJSON *pPlugins = cJSON_CreateArray();

    for(auto it = vInfo.begin(); it != vInfo.end(); ++it)
    {
        cJSON *pInfo = cJSON_CreateObject();
        cJSON *pId = cJSON_CreateNumber(it->nId);
        cJSON *pCode = cJSON_CreateNumber(it->nOperationResult);
        cJSON *pLog = cJSON_CreateString(it->sOperationLog.c_str());

        cJSON_AddItemToObject(pInfo, "plugins_id", pId);
        cJSON_AddItemToObject(pInfo, "code", pCode);
        cJSON_AddItemToObject(pInfo, "result", pLog);
        cJSON_AddItemToArray(pPlugins, pInfo);
    }
    cJSON_AddItemToObject(pRoot, "plugins", pPlugins);

    char *szPost = cJSON_Print(pRoot);

    utlLog_debug("report data = %s",szPost);

    RestClient::response r = RestClient::post(report_url, "application/json" , std::string(szPost), nTimeout);

    cJSON_Delete(pRoot);
    free(szPost);
    pRoot = NULL;

    if (r.code != 200)
        return false;

    pRoot = cJSON_Parse(r.body.c_str());

    if(pRoot == NULL)
        //parse error
        return false;

    cJSON *pResult = cJSON_GetObjectItem(pRoot, "result");
    if( pResult == NULL)
    {
        // log server return error
        cJSON_Delete(pRoot);
        return false;
    }

    std::string result = pResult->valuestring;
    if (result != "success")
    {
        // log(result) , server return error
        cJSON_Delete(pRoot);
        return false;
    }
    cJSON_Delete(pRoot);

    return true;
}

bool CPlugin::IsAlreadyInConf(const std::string &source_url)
{
    char buff[256] = {'\0'};
    std::ifstream fp(sOpkgConfFile.c_str());

    if (fp.is_open()) 
    {
        while(!fp.eof())
        {
            fp.getline(buff, sizeof(buff)); 
            if (strstr(buff, source_url.c_str()) != NULL) 
            {
                fp.close();
                return true;
            }
        }
        fp.close();
    }
    else
    {
        utlLog_error("open file error!");
    }

    return false;
}

void CPlugin::UpdateOpkgConf(const std::string &source_url)
{
    if (!access(sOpkgConfFile.c_str(), R_OK|W_OK)) 
        if (IsAlreadyInConf(source_url))
            return;

    std::ofstream fp(sOpkgConfFile.c_str(), std::ofstream::out | std::ofstream::app);
    if (fp.is_open()) 
    {
        fp << "src/gz hiwifi " + source_url << std::endl;
        fp.close();
    }
    else 
    {
        utlLog_error("open file error!");
    }
}

void CPlugin::UpdatePlugins(const std::string &mac, const std::string &md5, const std::string &server_url)
{
    std::string source_url("");
    std::vector<CRetrunedPluginInfo> vReturnedPluginInfo;
    std::vector<CRetrunedPluginInfo> install_list;
    std::vector<CRetrunedPluginInfo> remove_list;

    if (!SyncPluginList(mac, md5, server_url, source_url, vReturnedPluginInfo)) 
    {
        utlLog_error("sync plugin list with server error!");
        return;
    }

    if (source_url != "") 
        UpdateOpkgConf(source_url);

    for(auto it = vReturnedPluginInfo.begin(); it != vReturnedPluginInfo.end(); ++it)
    {
        if (it->nAction == ACTION_NONE)
            continue;
        else if(it->nAction == ACTION_REMOVE)
        {
            //remove this pakcage
            it->Remove();

            remove_list.push_back(*it);
        }
        else if(it->nAction == ACTION_INSTALL) 
        {
            // install this plugin
            it->Install();

            install_list.push_back(*it);
        }
    }

    // report install info
    Report(server_url + "/ap/info/plugin/install/callback/" + mac, install_list);

    // report remove info
    Report(server_url + "/ap/info/plugin/remove/callback/" + mac, remove_list);
}

void CPlugin::UpgradeClient(void)
{
    std::string remove_cmd("opkg remove opsp-client_hiwifi");
    system(remove_cmd.c_str());
    exit(0);
}
