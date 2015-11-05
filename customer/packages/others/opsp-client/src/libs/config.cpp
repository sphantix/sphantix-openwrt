#include <unistd.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_ini_parser.h>
#include "main.h"
#include "config.h"

UBOOL8 CConfig::HandleConfig(const char *group, const char *name, const char *value, void *cxdata)
{
    CConfig *pconfig = (CConfig *)cxdata;

    #define MATCH(g, n) utlStr_strcmp(group, g) == 0 && utlStr_strcmp(name, n) == 0
    if (MATCH("Log", "logdest")) {
        pconfig->sLogDest = value;
    } else if (MATCH("Log", "logpath")) {
        if (pconfig->sLogDest == "file") 
            pconfig->sLogPath = value;
    } else if (MATCH("Log", "logname")) {
        if (pconfig->sLogDest == "file") 
            pconfig->sLogName = value;
    } else if (MATCH("Log", "loglevel")) {
        pconfig->nLogLevel = atoi(value);
    } else if (MATCH("WSServer", "url")) {
        pconfig->sWSServerUrl = value;
    } else if (MATCH("WSServer", "port")) {
        pconfig->sWSServerPort = value;
    } else if (MATCH("WSServer", "path")) {
        pconfig->sWSServerPath = value;
    } else if (MATCH("HTTPServer", "url")) {
        pconfig->sHttpServerUrl = value;
    } else if (MATCH("HTTPServer", "port")) {
        pconfig->sHttpServerPort = value;
    } else {
        return FALSE;  /* unknown section/name, error */
    }
    return TRUE;
}

void CConfig::CheckConfig(void)
{
    if((sLogDest == "") || ((sLogDest != "file") && (sLogDest != "stderr")))
        goto error;
    if((sLogDest == "file") && ((sLogPath == "") || (sLogName == "")))
        goto error;
    if((sWSServerUrl == "") || (sWSServerPort == "") || (sWSServerPath == ""))
        goto error;
    if((sHttpServerUrl == "") || (sHttpServerPort == ""))
        goto error;

    return;

error:
    fprintf(stderr, "config file error, exit!\n");
    exit(E_CONFIGCHECKERR);
}

void CConfig::ParseConfig(const std::string &configfile)
{
    if (!access(configfile.c_str(), R_OK)) 
    {
        utl_ini_parse_file(configfile.c_str(), ';', '=', CConfig::HandleConfig, (void *)this);
    }
    else
    {
        fprintf(stderr, "config file dose not exist, exit!\n");
        exit(E_NOCONFIGFILE);
    }
}

void CConfig::InitConfig(const std::string &configfile)
{
    ParseConfig(configfile);
    CheckConfig();
}
