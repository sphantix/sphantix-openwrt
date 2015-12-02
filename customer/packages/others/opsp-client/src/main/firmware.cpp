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

void CFirmware::GetFirmwareName(std::string &firmware_url)
{
    std::size_t found = firmware_url.find_last_of("/");
    sFirmwareName = firmware_url.substr(found + 1);
    utlLog_debug("filename = %s", sFirmwareName.c_str());
}

void CFirmware::CleanUp(void)
{
    std::string cleanup_cmd = "rm -f " + sFirmwarePath + sFirmwareName;
    utlLog_debug("clean up ,cleanup_cmd = %s", cleanup_cmd.c_str());
    system(cleanup_cmd.c_str());
}

bool CFirmware::DownLoadFirmware(std::string &firmware_url)
{
    bool ret(false);

    GetFirmwareName(firmware_url);

    std::string dwonload_cmd = "wget -P " + sFirmwarePath + " -c " + firmware_url;
    utlLog_debug("dwonload_cmd = %s", dwonload_cmd.c_str());

    if(safe_system(dwonload_cmd.c_str()) != 0)
    {
        utlLog_error("dwonload error");
        ret = false;
    }
    else 
        ret = true;

    return ret;
}

bool CFirmware::GetFirmwareUrl(const std::string &mac, const std::string &md5, const std::string &server_url, std::string &source_url, std::string &firmware_url)
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

    cJSON *pSourceUrl = cJSON_GetObjectItem(pRoot, "source_url");
    if (pSourceUrl != NULL) 
        source_url = pSourceUrl->valuestring;

    // server error
    cJSON *pfirmware= cJSON_GetObjectItem(pRoot, "firmware");
    if (pfirmware == NULL)
    {
        cJSON_Delete(pRoot);
        return false;
    }
    else
        firmware_url = pfirmware->valuestring;

    cJSON_Delete(pRoot);
    return true;
}

void CFirmware::DoTruelyFirmwareUpgrade(void)
{
    system("sysupgrade -b /tmp/sysupgrade.tgz");

    std::string upgrade_cmd = "mtd -j /tmp/sysupgrade.tgz -r write " + sFirmwarePath + sFirmwareName + " firmware";
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
    std::string source_url("");
    std::string firmware_url("");

    //get firmware url from server
    if (!GetFirmwareUrl(mac, md5, server_url, source_url, firmware_url)) 
    {
        utlLog_error("get firmware url from server error!");
        return;
    }

    //update opkg source
    if (source_url != "") 
        UpdateOpkgConf(source_url);

    //download firmware
    if (firmware_url != "")
    {
        if (DownLoadFirmware(firmware_url)) 
        {
            utlLog_debug("download success, upgrade now!");
            DoTruelyFirmwareUpgrade();
        }
        else
            utlLog_error("download firmware failed!");

        CleanUp();
    }
    else
        utlLog_debug("firmware url is null!");

    return;
}
