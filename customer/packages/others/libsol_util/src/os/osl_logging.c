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

#include "osl_logging.h"

/** OS dependent logging functions go in this file.
 */
void oslLog_init(void)
{
   openlog(NULL, 0, LOG_DAEMON);
   return;
}

void oslLog_syslog(UtlLogLevel level, const char *buf)
{
   syslog(level, buf);
   return;
}

void oslLog_cleanup(void)
{
   closelog();
   return;
}
