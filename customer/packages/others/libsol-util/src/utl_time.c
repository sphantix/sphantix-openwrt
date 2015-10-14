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

#include "utl_time.h"
#include "osl_time.h"

void utlTm_get(UtlTimestamp *tms)
{
   oslTm_get(tms);
}

void utlTm_delta(const UtlTimestamp *newTms, const UtlTimestamp *oldTms, UtlTimestamp *deltaTms)
{
    if (newTms->sec >= oldTms->sec)
    {
        if (newTms->nsec >= oldTms->nsec)
        {
            /* no roll-over in the sec and nsec fields, straight subtract */
            deltaTms->nsec = newTms->nsec - oldTms->nsec;
            deltaTms->sec = newTms->sec - oldTms->sec;
        }
        else
        {
            /* no roll-over in the sec field, but roll-over in nsec field */
            deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
            deltaTms->sec = newTms->sec - oldTms->sec - 1;
        }
    }
    else
    {
        if (newTms->nsec >= oldTms->nsec)
        {
            /* roll-over in the sec field, but no roll-over in the nsec field */
            deltaTms->nsec = newTms->nsec - oldTms->nsec;
            deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec + 1; /* +1 to account for time spent during 0 sec */
        }
        else
        {
            /* roll-over in the sec and nsec fields */
            deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
            deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec;
        }
    }
}

UINT32 utlTm_deltaInMilliSeconds(const UtlTimestamp *newTms, const UtlTimestamp *oldTms)
{
   UtlTimestamp deltaTms;
   UINT32 ms;

   utlTm_delta(newTms, oldTms, &deltaTms);

   if (deltaTms.sec > MAX_UINT32 / MSECS_IN_SEC)
   {
      /* the delta seconds is larger than the UINT32 return value, so return max value */
      ms = MAX_UINT32;
   }
   else
   {
      ms = deltaTms.sec * MSECS_IN_SEC;

      if ((MAX_UINT32 - ms) < (deltaTms.nsec / NSECS_IN_MSEC))
      {
         /* overflow will occur when adding the nsec, return max value */
         ms = MAX_UINT32;
      }
      else
      {
         ms += deltaTms.nsec / NSECS_IN_MSEC;
      }
   }

   return ms;
}

void utlTm_addMilliSeconds(UtlTimestamp *tms, UINT32 ms)
{
   UINT32 addSeconds;
   UINT32 addNano;

   addSeconds = ms / MSECS_IN_SEC;
   addNano = (ms % MSECS_IN_SEC) * NSECS_IN_MSEC;

   tms->sec += addSeconds;
   tms->nsec += addNano;

   /* check for carry-over in nsec field */
   if (tms->nsec > NSECS_IN_SEC)
   {
      /* we can't have carried over by more than 1 second */
      tms->sec++;
      tms->nsec -= NSECS_IN_SEC;
   }

   return;
}
