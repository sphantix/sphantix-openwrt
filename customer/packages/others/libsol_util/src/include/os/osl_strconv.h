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

#ifndef __OSL_STRCONV_H__
#define __OSL_STRCONV_H__

#include "utl.h"

UtlRet osl_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val);
UtlRet osl_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val);
UtlRet osl_strtol64(const char *str, char **endptr, SINT32 base, SINT64 *val);
UtlRet osl_strtoul64(const char *str, char **endptr, SINT32 base, UINT64 *val);

#endif /* __OSL_STRCONV_H__ */
