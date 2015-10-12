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

#ifndef __UTL_TIMER_H__
#define __UTL_TIMER_H__

#include "utl.h"
#include "utl_time.h"
#include "rbtree.h"


/** Event handler type definition
 */
typedef void (*UtlEventHandler)(void*);


/** Max length (including NULL character) of an event timer name.
 *
 * When an event timer is created, the caller can give it a name
 * to help with debugging and lookup.  Name is optional.
 */
#define UTL_EVENT_TIMER_NAME_LENGTH  32

/** Internal event timer structure
 */
typedef struct utl_timer_event
{
    struct rb_node           node;      /**< node for rbtree. */
    UtlTimestamp             expireTms; /**< Timestamp (in the future) of when this timer event will expire. */
    UtlEventHandler          func;      /**< handler func to call when event expires. */
    void *                   ctxData;   /**< context data to pass to func */
    char name[UTL_EVENT_TIMER_NAME_LENGTH]; /**< name of this timer */
} UtlTimerEvent;

/** Internal timer handle. */
typedef struct
{
   struct rb_root  root;       /**< timer tree root node */
   UINT32          numEvents;  /**< Number of events in this handle. */
} UtlTimerHandle;


UtlRet utlTmr_init(void **tmrHandle);
UtlRet utlTmr_set(void *handle, UtlEventHandler func, void *ctxData, UINT32 ms, const char *name);
void utlTmr_cancel(void *handle, UtlEventHandler func, void *ctxData);
UtlRet utlTmr_getTimeToNextEvent(const void *handle, UINT32 *ms);
UINT32 utlTmr_getNumberOfEvents(const void *handle);
void utlTmr_executeExpiredEvents(void *handle);
UBOOL8 utlTmr_isEventPresent(const void *handle, UtlEventHandler func, void *ctxData);
void utlTmr_dumpEvents(const void *handle);
UtlRet utlTmr_replaceIfSooner(void *handle, UtlEventHandler func, void *ctxData, UINT32 ms, const char *name);
void utlTmr_cleanup(void **handle);

#endif /* __UTL_TIMER_H__ */
