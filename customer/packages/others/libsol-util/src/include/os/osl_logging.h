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

#ifndef __OSL_LOGGING_H__
#define __OSL_LOGGING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <syslog.h>
#include "../utl_logging.h"

void oslLog_init(void);
void oslLog_syslog(UtlLogLevel level, const char *buf);
void oslLog_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_LOGGING_H__ */
