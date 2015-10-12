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

#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include "osl_strconv.h"

UtlRet osl_strtol(const char *str, char **endptr, SINT32 base, SINT32 *val)
{
    UtlRet ret = UTLRET_SUCCESS;
    char *localEndPtr = NULL;

    errno = 0;  /* set to 0 so we can detect ERANGE */

    *val = strtol(str, &localEndPtr, base);

    if ((errno != 0) || (*localEndPtr != '\0'))
    {
        *val = 0;
        ret = UTLRET_INVALID_ARGUMENTS;
    }

    if (endptr != NULL)
    {
        *endptr = localEndPtr;
    }

    return ret;
}


UtlRet osl_strtoul(const char *str, char **endptr, SINT32 base, UINT32 *val)
{
    UtlRet ret = UTLRET_SUCCESS;
    char *localEndPtr = NULL;

    /*
     * Linux strtoul allows a minus sign in front of the number.
     * This seems wrong to me.  Specifically check for this and reject
     * such strings.
     */
    while (isspace(*str))
    {
        str++;
    }
    if (*str == '-')
    {
        if (endptr)
        {
            *endptr = (char *) str;
        }
        *val = 0;
        return UTLRET_INVALID_ARGUMENTS;
    }

    errno = 0;  /* set to 0 so we can detect ERANGE */

    *val = strtoul(str, &localEndPtr, base);

    if ((errno != 0) || (*localEndPtr != '\0'))
    {
        *val = 0;
        ret = UTLRET_INVALID_ARGUMENTS;
    }

    if (endptr != NULL)
    {
        *endptr = localEndPtr;
    }

    return ret;
}


UtlRet osl_strtol64(const char *str, char **endptr, SINT32 base, SINT64 *val)
{
    UtlRet ret = UTLRET_SUCCESS;
    char *localEndPtr = NULL;

    errno = 0;  /* set to 0 so we can detect ERANGE */

    *val = strtoll(str, &localEndPtr, base);

    if ((errno != 0) || (*localEndPtr != '\0'))
    {
        *val = 0;
        ret = UTLRET_INVALID_ARGUMENTS;
    }

    if (endptr != NULL)
    {
        *endptr = localEndPtr;
    }

    return ret;
}


UtlRet osl_strtoul64(const char *str, char **endptr, SINT32 base, UINT64 *val)
{
    UtlRet ret = UTLRET_SUCCESS;
    char *localEndPtr=NULL;

    /*
     * Linux strtoul allows a minus sign in front of the number.
     * This seems wrong to me.  Specifically check for this and reject
     * such strings.
     */
    while (isspace(*str))
    {
        str++;
    }
    if (*str == '-')
    {
        if (endptr)
        {
            *endptr = (char *) str;
        }
        *val = 0;
        return UTLRET_INVALID_ARGUMENTS;
    }

    errno = 0;  /* set to 0 so we can detect ERANGE */

    *val = strtoull(str, &localEndPtr, base);

    if ((errno != 0) || (*localEndPtr != '\0'))
    {
        *val = 0;
        ret = UTLRET_INVALID_ARGUMENTS;
    }

    if (endptr != NULL)
    {
        *endptr = localEndPtr;
    }

    return ret;
}
