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

#ifdef __cplusplus
extern "C" {
#endif

#include "utl.h"

/** handler type definition
 */
typedef UBOOL8 (*utl_ini_handler)(const char *group, const char *name, const char *value, void *cxdata);

UBOOL8 utl_ini_parse_buffer(char* buffer, char comment_char, char delim_char, utl_ini_handler handler, void *cxdata);

UBOOL8 utl_ini_parse_file(const char *filename, char comment_char, char delim_char, utl_ini_handler handler, void *cxdata);

#ifdef __cplusplus
}
#endif

#endif /* __UTL_INI_PARSER__ */
