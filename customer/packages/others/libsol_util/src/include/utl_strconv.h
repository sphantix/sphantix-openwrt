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

#ifndef __UTL_STRCONV_H__
#define __UTL_STRCONV_H__

#include "utl.h"

UtlRet utlStr_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val);
UtlRet utlStr_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val);
UtlRet utlStr_strtol64(const char *str, char **endptr, SINT32 base, SINT64 *val);
UtlRet utlStr_strtoul64(const char *str, char **endptr, SINT32 base, UINT64 *val);
void utlStr_strToLower(char *string);
void utlStr_strToUpper(char *string);
char *utlStr_strTrim(char* str);
SINT32 utlStr_strcmp(const char *s1, const char *s2);
SINT32 utlStr_strcasecmp(const char *s1, const char *s2);
SINT32 utlStr_strncmp(const char *s1, const char *s2, SINT32 n);
SINT32 utlStr_strncasecmp(const char *s1, const char *s2, SINT32 n);
char *utlStr_strstr(const char *s1, const char *s2);
char *utlStr_strncpy(char *dest, const char *src, SINT32 dlen);
char *utlStr_strcpy(char *dest, const char *src);
char *utlStr_strcat(char *dest, const char *src);
SINT32 utlStr_strlen(const char *src);
char *utlStr_getBaseName(char *path);
#endif /* __UTL_STRCONV_H__ */
