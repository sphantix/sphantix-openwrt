/***********************************************************************
 *
 *  Copyright (c) 2015 sphantix
 *  All Rights Reserved
 *
# 
# 
# 
#
 * 
 ************************************************************************/

#include <libsol-util/utl_logging.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_memory.h>
#include <libsol-util/utl_ini_parser.h>
#include "config.h"

UBOOL8 config_handler(const char *group, const char *name, const char *value, void *cxdata)
{
    config *pconfig = (config *)cxdata;

    #define MATCH(g, n) utlStr_strcmp(group, g) == 0 && utlStr_strcmp(name, n) == 0
    if (MATCH("Log", "logdest")) {
        pconfig->log_dest = utlMem_strdup(value);
    } else if (MATCH("Log", "logfile")) {
        if (!utlStr_strcmp(pconfig->log_dest, "file")) 
            pconfig->log_file = utlMem_strdup(value);
    } else if (MATCH("Log", "loglevel")) {
        pconfig->log_level = atoi(value);
    } else if (MATCH("Server", "url")) {
        pconfig->server_url = utlMem_strdup(value);
    } else if (MATCH("Server", "port")) {
        pconfig->server_port = atoi(value);
    } else if (MATCH("Server", "path")) {
        pconfig->server_path = utlMem_strdup(value);
    } else {
        return FALSE;  /* unknown section/name, error */
    }
    return TRUE;
}

void config_parse(config *ocsi_config, char *configfile)
{
    utl_ini_parse_file(configfile, ';', '=', config_handler, ocsi_config);
}

void config_cleanup(config *ocsi_config)
{
    UTLMEM_FREE_BUF_AND_NULL_PTR(ocsi_config->log_dest);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ocsi_config->log_file);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ocsi_config->server_url);
    UTLMEM_FREE_BUF_AND_NULL_PTR(ocsi_config->server_path);
}
