#include <csignal>
#include <unistd.h>
#include <libsol-util/utl_logging.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_memory.h>
#include <libsol-util/utl_ini_parser.h>
#include "main.h"
#include "client.h"
#include "ws_thread.h"
#include "handler_thread.h"
#include "restclient.h"
#include "curl/curl.h"
#include "cJSON.h"

void CClient::GetMac(void)
{
    FILE *fp = NULL;
    char buff[64] = {'\0'};
    char mac[16] = {'\0'};
    std::string cmd("ifconfig eth0 | grep HWaddr | awk {'print $NF'} | sed \"s/://g\"");

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        while(fgets(buff, sizeof(buff), fp) != NULL)
        {
            sscanf(buff, "%s\n", mac);
            sMac = mac;
            utlLog_debug("mac = %s", sMac.c_str());
        }
        pclose(fp);
    }
    else
    {
        utlLog_error("popen error, exit!");
        bKeepLooping = false;
    }
}

void CClient::GetKernelMD5(void)
{
    FILE *fp = NULL;
    char buff[128] = {'\0'};
    char kernelmd5[128] = {'\0'};
    std::string cmd("opkg info kernel | grep Version | awk -F\"-\" {'print $NF'}");

    if((fp = popen(cmd.c_str(), "r")) != NULL)
    {
        while(fgets(buff, sizeof(buff), fp) != NULL)
        {
            sscanf(buff, "%s\n", kernelmd5);
            sKernelMD5 = kernelmd5;
            utlLog_debug("KernelMD5 = %s", sKernelMD5.c_str());
        }
        pclose(fp);
    }
    else
    {
        utlLog_error("popen error, exit!");
        bKeepLooping = false;
    }
}

void CClient::InitSysInfo(void)
{
    GetMac();
    //wait for GetMac child process exit;
    sleep(1);
    GetKernelMD5();
}

void CClient::InitLog(void)
{
    utlLog_init();
    utlLog_setLevel((UtlLogLevel)config.nLogLevel);
    if (config.sLogDest == "file") 
    {
        utlLog_setDestination(LOG_DEST_FILE);
        config.sLogFile = config.sLogPath + config.sLogName;
        utlLog_setLogFile((char *)config.sLogFile.c_str());
    }
    else if (config.sLogDest == "stderr")
    {
        utlLog_setDestination(LOG_DEST_STDERR);
    }
}

std::vector<std::string> CClient::Split(const std::string str, const std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    std::string tmp_string = str + pattern;
    unsigned int size = tmp_string.size();

    for(unsigned int i=0; i<size; i++)
    {
        pos = tmp_string.find(pattern,i);
        if(pos < size)
        {
            std::string s = tmp_string.substr(i,pos-i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void CClient::InitWSServer(void)
{
    sWSServerFullPath = "ws://" + sServerUrl + ":80/ws";
}

void CClient::InitHTTPServer(void)
{
    sHttpServerFullPath = "http://" + sServerUrl + ":80";
}

void CClient::RandomSelectServerUrl(void)
{
    sServerUrl = vServerList[random()%vServerList.size()];
    InitWSServer();
    InitHTTPServer();

    utlLog_debug("selected server = %s", sServerUrl.c_str());
}

void CClient::InitServerInfo(void)
{
    vServerList = Split(OPSP_SERVER_CLUSTER, ";");
    RandomSelectServerUrl();
}

void CClient::Init(void)
{
    InitLog();
    InitServerInfo();
    InitSysInfo();
}

void CClient::CleanUp()
{
    if (tid_handler)
    {
        utlLog_debug("Explicitly killing the handler thread");
        pthread_cancel(tid_handler);
    }
    if (tid_ws)
    {
        utlLog_debug("Explicitly killing the websocket thread");
        pthread_cancel(tid_ws);
    }

    utlLog_debug("Exiting...");
    utlLog_cleanup();
}

bool CClient::IsFisrtBoot(void)
{
    if (access(sClientInitFlagFile.c_str(), F_OK) != 0) 
        return true;
    else 
        return false;
}

void CClient::FirstSyncWithServer(void)
{
    utlLog_debug("Enter initialize!");
    while(bKeepLooping)
    {
        std::string url = sHttpServerFullPath + "/ap/info/plugin/sync/" + sMac;
        utlLog_debug("url = %s", url.c_str());
        RestClient::response r = RestClient::get(url, 10);

        if (r.code != 200)
        {
            sleep(10);
            RandomSelectServerUrl();
            continue;
        }
        else 
        {
            cJSON *pRoot = cJSON_Parse(r.body.c_str());
            if (pRoot) 
            {
                cJSON *pResult = cJSON_GetObjectItem(pRoot, "result");
                if (pResult)
                {
                    char *szPost = cJSON_Print(pRoot);
                    utlLog_debug("buff = %s", szPost);
                    std::string result = pResult->valuestring;
                    if (result == "success")
                    {
                        std::string cmd = "touch " + sClientInitFlagFile;
                        system(cmd.c_str());
                        cJSON_Delete(pRoot);
                        break;
                    }
                }
                cJSON_Delete(pRoot);
                pRoot = NULL;
            }
        }
    }
}

void CClient::Run(void)
{
    //read & parse config file
    config.InitConfig(option.sConfigFile);

    //client init
    Init();

    if (IsFisrtBoot()) 
        FirstSyncWithServer();

    /* Start websocket thread */
    if (pthread_create(&tid_ws, NULL, thread_websocket, NULL) != 0)
    {
        utlLog_error("Failed to create a new thread (websocket) - exiting");
        bKeepLooping = false;
    }
    pthread_detach(tid_ws);

    /* Start http thread */
    if (pthread_create(&tid_handler, NULL, thread_handler, NULL) != 0)
    {
        utlLog_error("Failed to create a new thread (handler) - exiting");
        bKeepLooping = false;
    }
    pthread_detach(tid_handler);

    while(bKeepLooping)
    {
        sleep(10);
    }

    CleanUp();
}
