#include <libsol-util/utl_logging.h>
#include "http_interface.h"
#include "restclient.h"
#include "curl/curl.h"
#include "cJSON.h"

void CHttpInterface::Initialize(const std::string &server_url, const std::string &mac, const std::string &file)
{
    bool loop(true);

    utlLog_debug("Enter initialize!");
    while(loop)
    {
        std::string url = server_url + "/ap/info/plugin/sync/" + mac;
        utlLog_debug("url = %s", url.c_str());
        RestClient::response r = RestClient::get(url, 10);
        if (r.code == -1 || r.code == CURLE_OPERATION_TIMEDOUT )
        {
            sleep(5);
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
                        std::string cmd = "touch " + file;
                        system(cmd.c_str());
                        loop = false;
                    }
                }
                cJSON_Delete(pRoot);
            }
        }
    }
}
