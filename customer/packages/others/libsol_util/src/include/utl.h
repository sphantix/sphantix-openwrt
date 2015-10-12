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

#ifndef __UTL_H__
#define __UTL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"

typedef enum
{
    UTLRET_SUCCESS                  = 0,            /**<Success. */
    UTLRET_INVALID_ARGUMENTS        = 9003,         /**< Invalid arguments. */
    UTLRET_RESOURCE_EXCEEDED        = 9004,         /**< Resource exceeded. */
    UTLRET_NO_MORE_INSTANCES        = 9804,         /**<getnext operation cannot find any more instances to return. */

} UtlRet;

#ifndef __always_inline
#define __always_inline		inline __attribute__((always_inline))
#endif

/** Return byte offset of the specified member.
 *
 * This is defined in stddef.h for MIPS, but not defined
 * on LINUX desktop systems.  Play it safe and just define
 * it here for all build types.
 */
#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)


/** cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* __UTL_H__ */
