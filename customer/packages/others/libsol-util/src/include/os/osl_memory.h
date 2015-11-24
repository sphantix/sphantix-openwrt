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

#ifndef __OSL_MEMORY_H__
#define __OSL_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../utl.h"

void *osl_malloc(UINT32 size);
void osl_free(void *buf);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_MEMORY_H__ */
