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

#include <fcntl.h>      /* open */
#include "utl_logging.h"
#include "utl_prctl.h"
#include "utl_time.h"
#include "utl_strconv.h"
#include "utl_memory.h"
#include "osl_logging.h"

static UtlLogLevel                  logLevel;
static UtlLogDestination            logDestination;
static UINT32 logHeaderMask;
static char processName[256] = {'\0'};
static char *logFileName = NULL;

void utlLog_log(UtlLogLevel level, const char *func, UINT32 lineNum, const char *pFmt, ... )
{
    va_list		ap;
    char buf[MAX_LOG_LINE_LENGTH] = {0};
    int len=0, maxLen;
    char *logLevelStr=NULL;
    int logTelnetFd = -1;

    maxLen = sizeof(buf);

    if (level <= logLevel)
    {
        va_start(ap, pFmt);

        if (logHeaderMask & UTLLOG_HDRMASK_APPNAME)
        {
            if (utlStr_strlen(processName) != 0) 
                len = snprintf(buf, maxLen, "%s:", processName);
            else
                len = snprintf(buf, maxLen, "unkown:");
        }

        if ((logHeaderMask & UTLLOG_HDRMASK_LEVEL) && (len < maxLen))
        {
            /*
             * Only log the severity level when going to stderr
             * because syslog already logs the severity level for us.
             */
            if (logDestination == LOG_DEST_STDERR)
            {
                switch(level)
                {
                    case LOG_LEVEL_ERR:
                        logLevelStr = "error";
                        break;
                    case LOG_LEVEL_NOTICE:
                        logLevelStr = "notice";
                        break;
                    case LOG_LEVEL_DEBUG:
                        logLevelStr = "debug";
                        break;
                    default:
                        logLevelStr = "invalid";
                        break;
                }
                len += snprintf(&(buf[len]), maxLen - len, "%s:", logLevelStr);
            }
        }

        /*
         * Log timestamp for both stderr and syslog because syslog's
         * timestamp is when the syslogd gets the log, not when it was
         * generated.
         */
        if ((logHeaderMask & UTLLOG_HDRMASK_TIMESTAMP) && (len < maxLen))
        {
            UtlTimestamp ts;

            utlTm_get(&ts);
            len += snprintf(&(buf[len]), maxLen - len, "%u.%03u:",
                    ts.sec%1000, ts.nsec/NSECS_IN_MSEC);
        }

        if ((logHeaderMask & UTLLOG_HDRMASK_LOCATION) && (len < maxLen))
        {
            len += snprintf(&(buf[len]), maxLen - len, "%s:%u:", func, lineNum);
        }

        if (len < maxLen)
        {
            maxLen -= len;
            vsnprintf(&buf[len], maxLen, pFmt, ap);
        }

        if (logDestination == LOG_DEST_STDERR)
        {
            switch(level)
            {
                case LOG_LEVEL_ERR:
                    fprintf(stderr, "\033[1;31m%s\033[0m\n", buf);
                    break;
                case LOG_LEVEL_NOTICE:
                    fprintf(stderr, "\033[1;33m%s\033[0m\n", buf);
                    break;
                case LOG_LEVEL_DEBUG:
                    fprintf(stderr, "\033[1;32m%s\033[0m\n", buf);
                    break;
                default:
                    fprintf(stderr, "\033[1;32m%s\033[0m\n", buf);
                    break;
            }
            fflush(stderr);
        }
        else if (logDestination == LOG_DEST_TELNET)
        {
#ifdef DESKTOP_LINUX
            /* Fedora Desktop Linux */
            logTelnetFd = open("/dev/pts/1", O_RDWR);
#else
            /* CPE use ptyp0 as the first pesudo terminal */
            logTelnetFd = open("/dev/ttyp0", O_RDWR);
#endif
            if(logTelnetFd != -1)
            {
                write(logTelnetFd, buf, strlen(buf));
                write(logTelnetFd, "\n", strlen("\n"));
                close(logTelnetFd);
            }
        }
        else if (logDestination == LOG_DEST_SYSLOG)
        {
            oslLog_syslog(level, buf);
        }
        else
        {
            int logFileFd = -1;
            if (logFileName != NULL) 
            {
                logFileFd = open(logFileName, (O_WRONLY|O_CREAT|O_APPEND), 0644);

                if(logFileFd != -1)
                {
                    write(logFileFd, buf, strlen(buf));
                    write(logFileFd, "\n", strlen("\n"));
                    close(logFileFd);
                }
            }
        }

        va_end(ap);
    }
}

void utlLog_init(void)
{
    logLevel       = DEFAULT_LOG_LEVEL;
    logDestination = DEFAULT_LOG_DESTINATION;
    logHeaderMask  = DEFAULT_LOG_HEADER_MASK;

    oslLog_init();
    utlPrctl_getProcessName(processName);

    return;
}

void utlLog_cleanup(void)
{
    oslLog_cleanup();

    if (logFileName != NULL) 
    {
        utlMem_free(logFileName);
        logFileName = NULL;
    }

    return;
}

void utlLog_setLevel(UtlLogLevel level)
{
    logLevel = level;
    return;
}


UtlLogLevel utlLog_getLevel(void)
{
    return logLevel;
}

void utlLog_setDestination(UtlLogDestination dest)
{
    logDestination = dest;
    return;
}


UtlLogDestination utlLog_getDestination(void)
{
    return logDestination;
}


void utlLog_setHeaderMask(UINT32 headerMask)
{
    logHeaderMask = headerMask;
    return;
}


UINT32 utlLog_getHeaderMask(void)
{
    return logHeaderMask;
}

void utlLog_setLogFile(char *name)
{
    logFileName = utlMem_strdup(name);
    return;
}

char *utlLog_getLogFile(void)
{
    return logFileName;
}
