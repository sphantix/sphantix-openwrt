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

#ifndef __OSL_TIME_H__
#define __OSL_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "../utl.h"
#include "../utl_time.h"

void oslTm_get(UtlTimestamp *tms);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_TIME_H__ */
