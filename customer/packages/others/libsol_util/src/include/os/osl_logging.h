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

#include <syslog.h>
#include "utl_logging.h"

void oslLog_init(void);
void oslLog_syslog(UtlLogLevel level, const char *buf);
void oslLog_cleanup(void);

#endif /* __OSL_LOGGING_H__ */
