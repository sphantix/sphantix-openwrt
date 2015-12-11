#include <cstdio>
#include <string>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <curl/curl.h>
#include <libsol-util/utl_logging.h>
#include "main.h"
#include "firmware.h"
#include "cJSON.h"
#include "restclient.h"

void CFirmware::GetFileName(eFirmwareType type, std::string &url)
{
    std::size_t found = url.find_last_of("/");
    std::string filename = url.substr(found + 1);
    
    if (type == eUboot)
        sUbootName = filename;
    else if (type == eFirmware)
        sFirmwareName = filename;

    utlLog_debug("filename = %s", filename.c_str());
}

void CFirmware::CleanUp(eFirmwareType type)
{
    std::string cleanup_cmd("");

    if (type == eUboot)
        cleanup_cmd = "rm -f " + sFirmwarePath + sUbootName;
    else if (type == eFirmware)
        cleanup_cmd = "rm -f " + sFirmwarePath + sFirmwareName;

    if (cleanup_cmd != "")
    {
        utlLog_debug("clean up ,cleanup_cmd = %s", cleanup_cmd.c_str());
        system(cleanup_cmd.c_str());
    }
}

bool CFirmware::MD5Check(eFirmwareType type, std::string &md5sum)
{
    FILE *fp;
    char buff[256] = {'\0'};
    char md5[48] = {'\0'};
    char filename[128] = {'\0'};
    std::string md5check_cmd("");
    
    if (type == eUboot)
        md5check_cmd = "md5sum " + sFirmwarePath + sUbootName + " > /tmp/md5sum"; 
    else if (type == eFirmware)
        md5check_cmd = "md5sum " + sFirmwarePath + sFirmwareName + " > /tmp/md5sum"; 

    if (md5check_cmd == "")
    {
        utlLog_error("input type error!");
        return false;
    }

    safe_system(md5check_cmd.c_str());

    if ((fp = fopen("/tmp/md5sum", "r")) == NULL)
    {
        utlLog_error("md5sum check error!");
        return false;
    }

    if (fgets(buff, sizeof(buff), fp) == NULL)
    {
        utlLog_error("md5sum file is empty!");
        goto err;
    }

    sscanf(buff, "%s %s\n", md5, filename);

    if (md5sum != md5)
    {
        utlLog_error("%s md5sum does not equal, please check downloaded file", filename);
        goto err;
    }

    return true;

err:
    fclose(fp);
    safe_system("rm -f /tmp/firmware_md5sum");

    return false;
}

bool CFirmware::DownLoadFile(eFirmwareType type, std::string &url, std::string &md5sum)
{
    GetFileName(type, url);

    std::string dwonload_cmd = "wget -P " + sFirmwarePath + " -c " + url;
    utlLog_debug("dwonload_cmd = %s", dwonload_cmd.c_str());

    if(safe_system(dwonload_cmd.c_str()) != 0)
    {
        utlLog_error("dwonload error");
        return false;
    }

    if (MD5Check(type, md5sum) != true)
    {
        utlLog_error("md5 check error");
        return false;
    }

    return true;
}

bool CFirmware::GetUpgradeInfo(const std::string &mac, const std::string &md5, const std::string &server_url, CUpgradeInfo &upgrade_info)
{
    //combine json data
    cJSON *pRoot = cJSON_CreateObject();
    cJSON *pMac = cJSON_CreateString(mac.c_str());
    cJSON_AddItemToObject(pRoot, "mac", pMac);
    cJSON *pMD5 = cJSON_CreateString(md5.c_str());
    cJSON_AddItemToObject(pRoot, "kernelMD5", pMD5);

    char *szPost = cJSON_Print(pRoot);

    utlLog_debug("buff = %s", szPost);

    utlLog_debug("server_url = %s/ap/info/firmware/sync", server_url.c_str());

    //send to http server
    RestClient::response r = RestClient::post(server_url + "/ap/info/firmware/sync", "application/json" , std::string(szPost), nTimeout);
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
    cJSON *pFirmwareUrl = cJSON_GetObjectItem(pRoot, "firmware");
    if (pFirmwareUrl == NULL)
    {
        cJSON_Delete(pRoot);
        return false;
    }
    else
        upgrade_info.sFirmwareUrl = pFirmwareUrl->valuestring;

    cJSON *pFirmware_md5 = cJSON_GetObjectItem(pRoot, "firmware_md5");
    if (pFirmware_md5 == NULL)
    {
        cJSON_Delete(pRoot);
        return false;
    }
    else
        upgrade_info.sFirmwareMd5 = pFirmware_md5->valuestring;

    cJSON *pSaveConfig = cJSON_GetObjectItem(pRoot, "save_config");
    if (pSaveConfig == NULL)
    {
        cJSON_Delete(pRoot);
        return false;
    }
    else
        upgrade_info.nSaveConfig = pSaveConfig->valueint;

    //optional sections
    cJSON *pUbootUrl= cJSON_GetObjectItem(pRoot, "uboot");
    if (pUbootUrl != NULL)
        upgrade_info.sUbootUrl = pUbootUrl->valuestring; 

    cJSON *pUboot_md5 = cJSON_GetObjectItem(pRoot, "uboot_md5");
    if (pUboot_md5 != NULL)
        upgrade_info.sUbootMd5 = pUboot_md5->valuestring;

    cJSON *pSourceUrl = cJSON_GetObjectItem(pRoot, "source_url");
    if (pSourceUrl != NULL) 
        upgrade_info.sSourceUrl = pSourceUrl->valuestring;

    cJSON_Delete(pRoot);
    return true;
}

void CFirmware::DoTruelyUpgrade(eFirmwareType type, int save_config)
{
    std::string upgrade_cmd("");

    if (type == eUboot)
    {
        upgrade_cmd = "mtd write " + sFirmwarePath + sUbootName + " Bootloader";
    }
    else if (type == eFirmware)
    {
        if (save_config)
        {
            system("rm -f /etc/dnsmasq.conf");
            system("sysupgrade -b /tmp/sysupgrade.tgz");
            upgrade_cmd = "mtd -j /tmp/sysupgrade.tgz -r write " + sFirmwarePath + sFirmwareName + " firmware";
        }
        else
            upgrade_cmd = "mtd -r write " + sFirmwarePath + sFirmwareName + " firmware";
    }

    if (upgrade_cmd == "")
    {
        utlLog_error("input type error!");
        return;
    }

    utlLog_debug("upgrade_cmd = %s", upgrade_cmd.c_str());

    if(safe_system(upgrade_cmd.c_str()) != 0)
    {
        utlLog_error("sysupgrade error!");
    }
}

bool CFirmware::IsAlreadyInConf(const std::string &source_url)
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

void CFirmware::UpdateOpkgConf(const std::string &source_url)
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

void CFirmware::UpgradeFirmware(const std::string &mac, const std::string &md5, const std::string &server_url)
{
    CUpgradeInfo upgrade_info;

    //get firmware url from server
    if (!GetUpgradeInfo(mac, md5, server_url, upgrade_info)) 
    {
        utlLog_error("get firmware url from server error!");
        return;
    }

    //update opkg source
    if (upgrade_info.sSourceUrl != "") 
        UpdateOpkgConf(upgrade_info.sSourceUrl);

    //upgrade uboot
    if ((upgrade_info.sUbootUrl != "") && (upgrade_info.sUbootMd5 != ""))
    {
        if (DownLoadFile(eUboot, upgrade_info.sUbootUrl, upgrade_info.sUbootMd5)) 
        {
            utlLog_debug("download uboot success, upgrade now!");
            DoTruelyUpgrade(eUboot, 0);
        }
        else
            utlLog_error("download uboot failed!");

        CleanUp(eUboot);
    }
    else
        utlLog_debug("skip uboot upgrade");
        

    //download firmware
    if ((upgrade_info.sFirmwareUrl != "") && (upgrade_info.sFirmwareMd5 != ""))
    {
        if (DownLoadFile(eFirmware, upgrade_info.sFirmwareUrl, upgrade_info.sFirmwareMd5)) 
        {
            utlLog_debug("download success, upgrade now!");
            DoTruelyUpgrade(eFirmware, upgrade_info.nSaveConfig);
        }
        else
            utlLog_error("download firmware failed!");

        CleanUp(eFirmware);
    }
    else
        utlLog_debug("firmware parameter missing, upgrade fail!");

    return;
}
