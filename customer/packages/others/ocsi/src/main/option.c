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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libsol-util/utl_memory.h>
#include "option.h"

static const char *options = "c:vh";
static const char* version = "1.0.1";

void print_usage(void)
{
    printf("Usage: ocsi [cvh]\n");
    printf("-c:      config file\n");
    printf("-v:      print ocsi version\n");
    printf("-h:      print help & usage\n");
}

int option_parse(option *op, int argc, char **argv){
    int opt = 0;
    opt = getopt(argc, argv, options);
    while( opt != -1 ){
        switch( opt ){
            case 'c':
                op->configfile = utlMem_strdup(optarg);
                break;
            case 'v':
                printf("%s\r\n", version);
                break;
            case 'h':
                print_usage();
                break;
            default:
                break;
        }
        opt = getopt(argc, argv, options);
    }
    return 0;
}

void option_cleanup(option *op)
{
    UTLMEM_FREE_BUF_AND_NULL_PTR(op->configfile);
}
