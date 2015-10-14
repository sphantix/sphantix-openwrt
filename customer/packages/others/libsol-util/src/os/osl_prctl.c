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
#include "osl_prctl.h"
#include "utl_logging.h"

void oslPrctl_getPathName(pid_t pid, char *name)
{
    FILE *fp;
    char cmd[BUFLEN_256] = {'\0'};

    sprintf(cmd,"readlink /proc/%d/exe",pid);
    if((fp = popen(cmd,"r")) != NULL)
        if(fscanf(fp,"%s\n",name) != -1)
            ;

    pclose(fp);
    return;
}

void oslPrctl_signalProcess(SINT32 pid, SINT32 sig)
{
   SINT32 rc;

   if (pid <= 0)
   {
      utlLog_error("bad pid %d", pid);
      return;
   }

   if ((rc = kill(pid, sig)) < 0)
   {
      utlLog_error("invalid signal(%d) or pid(%d)", sig, pid);
      return;
   }

   return;
}

