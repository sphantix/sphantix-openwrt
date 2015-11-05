#include <string>
#include <libsol-util/utl_logging.h>
#include "main.h"
#include "plugin_info.h"

void CRetrunedPluginInfo::GetDownloadFileName(void)
{
    std::size_t found = sUrl.find_last_of("/");
    sDownloadFileName = sUrl.substr(found + 1);
    utlLog_debug("filename = %s", sDownloadFileName.c_str());
}

bool CRetrunedPluginInfo::CheckPluginUpdated(void)
{
    FILE *fp = NULL;
    char buff[128] = {'\0'};
    char name[32] = {'\0'};
    char version[128] = {'\0'};
    std::string cmd = "opkg list | grep " + sName;

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        if(fgets(buff, sizeof(buff), fp) != NULL)
        {
            sscanf(buff, "%s - %s\n", name, version);
            if (sName == name && sVersion == version) 
            {
                pclose(fp);
                return true;
            }
        }
        else
        {
            pclose(fp);
            return false;
        }
    }
    else
        return false;

    return false;
}

bool CRetrunedPluginInfo::CheckPluginExist(void)
{
    FILE *fp = NULL;
    char buff[128] = {'\0'};
    std::string cmd = "opkg list | grep " + sName;

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        if(fgets(buff, sizeof(buff), fp) != NULL)
        {
            pclose(fp);
            return true;
        }
        else
        {
            pclose(fp);
            return false;
        }
    }
    else
        return false;

    return false;
}

bool CRetrunedPluginInfo::DoOpkgCmd(const std::string &cmd)
{
    FILE *fp = NULL;
    char buff[128] = {'\0'};

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        while(fgets(buff, sizeof(buff), fp) != NULL)
        {
            if (strstr(buff, "error") != NULL) 
            {
                pclose(fp);
                return false;
            }
        }
        pclose(fp);
    }
    else
        return false;

    return true;
}

void CRetrunedPluginInfo::CleanUp(void)
{
    std::string cleanup_cmd = "rm -rf " + sLocalSavePath + sDownloadFileName;
    utlLog_debug("clean up ,cleanup_cmd = %s", cleanup_cmd.c_str());
    system(cleanup_cmd.c_str());
}

void CRetrunedPluginInfo::Install(void)
{
    std::string dwonload_cmd = "wget -P " + sLocalSavePath + " -c " + sUrl;

    utlLog_debug("Install ipk , dwonload_cmd = %s", dwonload_cmd.c_str());

    //get file name to save
    GetDownloadFileName();

    //download package
    if(system(dwonload_cmd.c_str()) != 0)
    {
        nOperationResult = E_DOWNLOADERR;
        sOperationLog = "Download package error!";
    }
    else
    {
        std::string install_cmd = "opkg install " + sLocalSavePath + sDownloadFileName;
        utlLog_debug("install_cmd = %s", install_cmd.c_str());
        //install package
        if (!DoOpkgCmd(install_cmd))
        {
            nOperationResult = E_OPKG_CMDERR;
            sOperationLog = "Do command error, opkg install failed!";
        }
        else
        {
            if (CheckPluginUpdated()) 
            {
                nOperationResult = E_SUCCESS;
                sOperationLog = "Install plugin success";
            }
            else
            {
                nOperationResult = E_IPK_INSTALLERR;
                sOperationLog = "Version error, opkg install failed!";
            }
        }
    }
    CleanUp();
}

void CRetrunedPluginInfo::Remove(void)
{
    std::string remove_cmd = "opkg remove " + sName;

    utlLog_debug("Remove ipk, cmd = %s", remove_cmd.c_str());

    //install package
    if (!DoOpkgCmd(remove_cmd))
    {
        nOperationResult = E_OPKG_CMDERR;
        sOperationLog = "Do command error, opkg remove failed!";
    }
    else
    {
        if (!CheckPluginExist()) 
        {
            nOperationResult = E_SUCCESS;
            sOperationLog = "Remove plugin success";
        }
        else
        {
            nOperationResult = E_IPK_REMOVEERR;
            sOperationLog = "Remove failed, opkg remove failed!";
        }
    }
}

void CRetrunedPluginInfo::ReInstall(void)
{
    nOperationResult = 0;
    sOperationLog = "success";

    utlLog_debug("ReInstall ipk");
}
