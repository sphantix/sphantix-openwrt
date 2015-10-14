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

#include "utl_strconv.h"
#include "utl_prctl.h"
#include "osl_prctl.h"

void utlPrctl_getProcessName(char *name)
{
    char path[BUFLEN_256] = {'\0'};

    oslPrctl_getPathName(getpid(), path);
    utlStr_strcpy(name, utlStr_getBaseName(path));

    return;
}

void utlPrctl_signalProcess(SINT32 pid, SINT32 sig)
{
    oslPrctl_signalProcess(pid, sig);
    return;
}
