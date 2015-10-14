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

#ifndef __UTL_INI_PARSER__
#define __UTL_INI_PARSER__

#include "utl.h"
#include "utl_list.h"

typedef struct
{
    DlistNode pair_node;
    char *key;
    char *value;        
} UtlIniValuePair;

typedef struct
{
    DlistNode group_node;
    DlistNode pair_head;
    char *group_name;
    int  pair_num;
} UtlIniGroup;

/** handler type definition
 */
typedef UBOOL8 (*UtlIniValueHandler)(DlistNode *);

UBOOL8 utl_ini_parser(char* buffer, char comment_char, char delim_char, UtlIniValueHandler func);
#endif /* __UTL_INI_PARSER__ */
