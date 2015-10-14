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

#include "osl_time.h"
#include "utl_logging.h"

void oslTm_get(UtlTimestamp *tms)
{
   SINT32 rc;
   struct timeval ts;

   if (tms == NULL)
   {
      return;
   }

   rc = gettimeofday(&ts, 0);
   if (rc == 0)
   {
      tms->sec = ts.tv_sec;
      tms->nsec = ts.tv_usec*1000;
   }
   else
   {
      utlLog_error("clock_gettime failed, set timestamp to 0");
      tms->sec = 0;
      tms->nsec = 0;
   }
}
