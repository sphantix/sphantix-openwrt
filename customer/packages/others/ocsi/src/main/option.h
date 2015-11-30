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

#ifndef __OPTION_H__
#define __OPTION_H__

#include <string.h>

typedef struct {
    char *configfile;
} option;

int option_parse(option *op, int argc, char **argv);
void option_cleanup(option *op);

#endif /*__OPTION_H__ */
