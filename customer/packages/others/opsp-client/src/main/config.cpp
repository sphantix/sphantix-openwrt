#include <libsol-util/utl_logging.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_memory.h>
#include <libsol-util/utl_ini_parser.h>
#include "config.hpp"

UBOOL8 config::config_handler(const char *group, const char *name, const char *value, void *cxdata)
{
    config *pconfig = (config *)cxdata;

    #define MATCH(g, n) utlStr_strcmp(group, g) == 0 && utlStr_strcmp(name, n) == 0
    if (MATCH("Log", "logdest")) {
        pconfig->log_dest = value;
    } else if (MATCH("Log", "logpath")) {
        if (pconfig->log_dest == "file") 
            pconfig->log_path = value;
    } else if (MATCH("Log", "logname")) {
        if (pconfig->log_dest == "file") 
            pconfig->log_name = value;
    } else if (MATCH("Log", "loglevel")) {
        pconfig->log_level = atoi(value);
    } else if (MATCH("Server", "url")) {
        pconfig->server_url = value;
    } else if (MATCH("Server", "path")) {
        pconfig->server_path = value;
    } else {
        return FALSE;  /* unknown section/name, error */
    }
    return TRUE;
}

void config::config_parse(const std::string &configfile)
{
    utl_ini_parse_file(configfile.c_str(), ';', '=', config::config_handler, (void *)this);
}
