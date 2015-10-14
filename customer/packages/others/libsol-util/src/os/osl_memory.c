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

#include <stdlib.h>
#include "osl_memory.h"


void *osl_malloc(UINT32 size)
{
   return malloc(size);
}


void osl_free(void *buf)
{
   free(buf);
}
