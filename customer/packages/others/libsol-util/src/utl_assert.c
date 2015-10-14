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

#include <signal.h>
#include "utl_assert.h"
#include "utl_prctl.h"
#include "utl_logging.h"

void utlAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr)
{
    if (expr == 0)
    {
        utlLog_error("assertion \"%s\" failed at %s:%d", exprString, filename, lineNumber);

#ifndef NDEBUG
        /* Send SIGABRT only if NDEBUG is not defined */
        utlPrctl_signalProcess(getpid(), SIGABRT);
#endif
    }
}
