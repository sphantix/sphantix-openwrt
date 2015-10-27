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

#ifndef __UTL_PRCTL_H__
#define __UTL_PRCTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "utl.h"
#include "./os/osl_prctl.h"

void utlPrctl_getProcessName(char *name);
void utlPrctl_signalProcess(SINT32 pid, SINT32 sig);

#ifdef __cplusplus
}
#endif

#endif /* __UTL_PRCTL_H__ */
