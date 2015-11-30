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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <libsol-util/utl.h>

typedef struct {
    int log_level;
    char *log_dest;
    char *log_file;
    char *server_url;
    int   server_port;
    char *server_path;
} config;

void config_parse(config *ocsi_config, char *configfile);
void config_cleanup(config *ocsi_config);

#endif /* __CONFIG_H__ */
