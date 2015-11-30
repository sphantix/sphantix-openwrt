#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libsol-util/utl.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_logging.h>
#include "option.h"
#include "config.h"

void ocsi_log_init(config *ocsi_config)
{
    utlLog_init();
    utlLog_setLevel((UtlLogLevel)ocsi_config->log_level);
    if(!utlStr_strcmp(ocsi_config->log_dest, "file"))
    {
        utlLog_setDestination(LOG_DEST_FILE);
        utlLog_setLogFile(ocsi_config->log_file);
    }
    else if (!utlStr_strcmp(ocsi_config->log_dest, "stderr")) 
    {
        utlLog_setDestination(LOG_DEST_STDERR);
    }
}

void ocsi_server_init(config *ocsi_config)
{
    return;
}

void ocsi_init(config *ocsi_config)
{
    ocsi_log_init(ocsi_config);
    ocsi_server_init(ocsi_config);
}

void ocsi_cleanup(void)
{
    utlLog_cleanup();
    return;
}

int main(int argc, char *argv[])
{
    //init
    option op;
    config cfg;

    option_parse(&op, argc, argv);
    config_parse(&cfg, op.configfile);
    ocsi_init(&cfg);

    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");

    //clean up before exit
    ocsi_cleanup();
    config_cleanup(&cfg);
    option_cleanup(&op);

    return 0;
}
