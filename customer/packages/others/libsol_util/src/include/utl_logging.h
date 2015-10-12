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

#ifndef __UTL_LOGGING_H__
#define __UTL_LOGGING_H__

#include <stdarg.h>
#include <unistd.h>
#include "utl.h"

/*!\file utl_log.h
 * \brief Public header file for Management System Logging API.
 * Applications which need to call Logging API functions must
 * include this file.
 */

typedef enum
{
   LOG_LEVEL_ERR    = 3, /**< Log at error level. */
   LOG_LEVEL_NOTICE = 5, /**< Log at notice level. */
   LOG_LEVEL_DEBUG  = 7  /**< Log at debug level. */
} UtlLogLevel;

typedef enum
{
   LOG_DEST_STDERR  = 1,  /**< Log output to stderr. */
   LOG_DEST_SYSLOG  = 2,  /**< Log output to syslog. */
   LOG_DEST_TELNET  = 3,  /**< Log output to telnet clients. */
   LOG_DEST_FILE    = 4,  /**< Log output to telnet clients. */
} UtlLogDestination;

/** Show application name in the log line. */
#define UTLLOG_HDRMASK_APPNAME    0x0001 

/** Show log level in the log line. */
#define UTLLOG_HDRMASK_LEVEL      0x0002 

/** Show timestamp in the log line. */
#define UTLLOG_HDRMASK_TIMESTAMP  0x0004

/** Show location (function name and line number) level in the log line. */
#define UTLLOG_HDRMASK_LOCATION   0x0008 

#define DEFAULT_LOG_LEVEL        LOG_LEVEL_ERR

#define DEFAULT_LOG_DESTINATION  LOG_DEST_STDERR

#define UTL_STDOUT               STDOUT_FILENO

#define UTL_SRDERR               STDERR_FILENO

/** Default log header mask */
#define DEFAULT_LOG_HEADER_MASK (UTLLOG_HDRMASK_APPNAME|UTLLOG_HDRMASK_LEVEL|UTLLOG_HDRMASK_TIMESTAMP|UTLLOG_HDRMASK_LOCATION)

#define MAX_LOG_LINE_LENGTH      512

#define utlLog_debug(args...)  utlLog_log(LOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, args)
#define utlLog_notice(args...) utlLog_log(LOG_LEVEL_NOTICE, __FUNCTION__, __LINE__, args)
#define utlLog_error(args...)  utlLog_log(LOG_LEVEL_ERR, __FUNCTION__, __LINE__, args)

void utlLog_log(UtlLogLevel level, const char *func, UINT32 lineNum, const char *pFmt, ... );

void utlLog_init(void);

void utlLog_cleanup(void);

void utlLog_setLevel(UtlLogLevel level);

UtlLogLevel utlLog_getLevel(void);

void utlLog_setDestination(UtlLogDestination dest);

UtlLogDestination utlLog_getDestination(void);

void utlLog_setHeaderMask(UINT32 headerMask);

UINT32 utlLog_getHeaderMask(void);

void utlLog_setLogFile(char *name);

char *utlLog_getLogFile(void);
#endif /* __UTL_LOGGING_H__ */
