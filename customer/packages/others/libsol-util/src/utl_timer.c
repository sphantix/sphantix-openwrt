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

#include "utl_timer.h"
#include "utl_memory.h"
#include "utl_logging.h"

UtlRet utlTmr_init(void **tmrHandle)
{

    (*tmrHandle) = utlMem_alloc(sizeof(UtlTimerHandle), ALLOC_ZEROIZE);
    if ((*tmrHandle) == NULL)
    {
        utlLog_error("could not malloc mem for tmrHandle");
        return UTLRET_RESOURCE_EXCEEDED;
    }

    return UTLRET_SUCCESS;
}

/** This macro will evaluate TRUE if a is earlier than b */
#define IS_EARLIER_THAN(a, b) (((a)->sec < (b)->sec) || \
                               (((a)->sec == (b)->sec) && ((a)->nsec < (b)->nsec)))

/**
 * Adds timer to rbtree.
 *
 * Adds the timer node to the rbtree, sorted by the
 * node's expires value.
 */
static void utlTmr_add(void *handle, UtlTimerEvent *event)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    struct rb_node **p = &tmrHandle->root.rb_node;
    struct rb_node *parent = NULL;
    UtlTimerEvent *ptr;

    if(!RB_EMPTY_NODE(&event->node))
    {
        while (*p) {
            parent = *p;
            ptr = rb_entry(parent, UtlTimerEvent, node);
            if (IS_EARLIER_THAN(&(event->expireTms), &(ptr->expireTms)))
                p = &(*p)->rb_left;
            else
                p = &(*p)->rb_right;
        }
        rb_link_node(&event->node, parent, p);
        rb_insert_color(&event->node, &tmrHandle->root);
    }

    return;
}

/**
 * Removes a timer from the timerqueue.
 *
 * Removes the timer node from the timerqueue.
 */
static void utlTmr_del(void *handle, UtlTimerEvent *event)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    struct rb_root *tmr_tree = &tmrHandle->root;

    rb_erase(&event->node, tmr_tree);
    RB_CLEAR_NODE(&event->node);
    UTLMEM_FREE_BUF_AND_NULL_PTR(event);

    return;
}

static UtlTimerEvent *utlTmr_find(const void *handle, UtlEventHandler func, void *ctxData)
{
    const UtlTimerHandle *tmrHandle = (const UtlTimerHandle *) handle;
    const struct rb_root *tmr_tree = &tmrHandle->root;
    UtlTimerEvent *tmrEvent, *next;

    rbtree_postorder_for_each_entry_safe(tmrEvent, next, tmr_tree, node)
    {
        if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
            return tmrEvent;
    }

    return NULL;
}

UtlRet utlTmr_set(void *handle, UtlEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    UtlTimerEvent *newEvent;

    /*
     * First verify there is not a duplicate event.
     * (The original code first deleted any existing timer,
     * which is a "side-effect", bad style, but maybe tr69c requires
     * that functionality?)
     */
    if (utlTmr_isEventPresent(handle, func, ctxData))
    {
        utlLog_error("There is already an event func 0x%x ctxData 0x%x", func, ctxData);
        return UTLRET_INVALID_ARGUMENTS;
    }

    /* make sure name is not too long */
    if ((name != NULL) && (strlen(name) >= UTL_EVENT_TIMER_NAME_LENGTH))
    {
        utlLog_error("name of timer event is too long, max %d", UTL_EVENT_TIMER_NAME_LENGTH);
        return UTLRET_INVALID_ARGUMENTS;
    }

    /*
     * Allocate a structure for the timer event.
     */
    newEvent = utlMem_alloc(sizeof(UtlTimerEvent), ALLOC_ZEROIZE);
    if (newEvent == NULL)
    {
        utlLog_error("malloc of new timer event failed");
        return UTLRET_RESOURCE_EXCEEDED;
    }

    /* fill in fields of new event timer structure. */
    newEvent->func = func;
    newEvent->ctxData = ctxData;

    utlTm_get(&(newEvent->expireTms));
    utlTm_addMilliSeconds(&(newEvent->expireTms), ms);

    if (name != NULL)
    {
        sprintf(newEvent->name, "%s", name);
    }

    utlTmr_add(handle, newEvent);

    tmrHandle->numEvents++;

    utlLog_debug("added event %s, expires in %ums (at %u.%03u), func=0x%x data=%p count=%d",
            newEvent->name,
            ms,
            newEvent->expireTms.sec,
            newEvent->expireTms.nsec/NSECS_IN_MSEC,
            func,
            ctxData,
            tmrHandle->numEvents);

    return UTLRET_SUCCESS;
}

void utlTmr_cancel(void *handle, UtlEventHandler func, void *ctxData)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    UtlTimerEvent *tmrEvent;

    if (RB_EMPTY_ROOT(&tmrHandle->root))
    {
        utlLog_debug("no events to delete (func=0x%x data=%p)", func, ctxData);
        return;
    }

    if ((tmrEvent = utlTmr_find(handle, func, ctxData)) != NULL)
    {
        tmrHandle->numEvents--;
        utlLog_debug("canceled event %s, count=%d", tmrEvent->name, tmrHandle->numEvents);
        utlTmr_del(handle, tmrEvent);
    }
    else
    {
        utlLog_debug("could not find requested event to delete, func=0x%x data=%p count=%d", func, ctxData, tmrHandle->numEvents);
    }

    return;
}

static void utlTmr_destory(void *handle, struct rb_node *rbtree)
{
    UtlTimerEvent *event;

    if (rbtree == NULL)
    {
        return;
    }

    if(rbtree->rb_left != NULL)
        utlTmr_destory(handle, rbtree->rb_left);
    if(rbtree->rb_right != NULL)
        utlTmr_destory(handle, rbtree->rb_right);

    event = rb_entry(rbtree, UtlTimerEvent, node);
    utlTmr_del(handle, event);
}

UtlRet utlTmr_getTimeToNextEvent(const void *handle, UINT32 *ms)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    struct rb_node **p = &tmrHandle->root.rb_node;
    UtlTimerEvent *currEvent;
    UtlTimestamp nowTms;

    utlTm_get(&nowTms);

    if (*p == NULL)
    {
        *ms = MAX_UINT32;
        return UTLRET_NO_MORE_INSTANCES;
    }

    while((*p)->rb_left)
    {
        p = &(*p)->rb_left;
    }

    currEvent = rb_entry(*p, UtlTimerEvent, node);

    /* this is the same code as in dumpEvents, integrate? */
    if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
    {
        /*
         * the next event is past due (nowTms is later than currEvent),
         * so time to next event is 0.
         */
        *ms = 0;
    }
    else
    {
        /*
         * nowTms is earlier than currEvent, so currEvent is still in
         * the future.  
         */
        (*ms) = utlTm_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
    }

    return UTLRET_SUCCESS;
}

static UtlTimerEvent *utlTmr_getNextEvent(const void *handle)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    struct rb_node **p = &tmrHandle->root.rb_node;
    UtlTimerEvent *currEvent;
    UtlTimestamp nowTms;

    utlTm_get(&nowTms);

    if (*p == NULL)
    {
        return NULL;
    }

    while((*p)->rb_left)
    {
        p = &(*p)->rb_left;
    }

    currEvent = rb_entry(*p, UtlTimerEvent, node);

    return currEvent;
}

UINT32 utlTmr_getNumberOfEvents(const void *handle)
{
    const UtlTimerHandle *tmrHandle = (const UtlTimerHandle *) handle;

    return (tmrHandle->numEvents);
}

void utlTmr_executeExpiredEvents(void *handle)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) handle;
    UtlTimerEvent *currEvent;
    UtlTimestamp nowTms;

    utlTm_get(&nowTms);

    currEvent = utlTmr_getNextEvent(handle);

    while ((currEvent != NULL) && (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms)))
    {
        /*
         * first remove the currEvent from the tmrHandle because
         * when we execute the callback function, it might call the
         * cmsTmr API again.
         */
        tmrHandle->numEvents--;

        utlLog_debug("executing timer event %s func 0x%x data 0x%x",
                currEvent->name, currEvent->func, currEvent->ctxData);

        /* call the function */
        (*currEvent->func)(currEvent->ctxData);

        /* delete executed timer from rbtree */
        utlTmr_del(handle, currEvent);

        currEvent = utlTmr_getNextEvent(handle);
    }

    return;
}

UBOOL8 utlTmr_isEventPresent(const void *handle, UtlEventHandler func, void *ctxData)
{
    const UtlTimerHandle *tmrHandle = (const UtlTimerHandle *) handle;
    const struct rb_root *tmr_tree = &tmrHandle->root;
    UtlTimerEvent *tmrEvent, *next;
    UBOOL8 found = FALSE;

    if (func != NULL) 
    {
        rbtree_postorder_for_each_entry_safe(tmrEvent, next, tmr_tree, node)
        {
            if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
            {
                found = TRUE;
                break;
            }
        }
    }

    return found;
}

void utlTmr_dumpEvents(const void *handle)
{
    const UtlTimerHandle *tmrHandle = (const UtlTimerHandle *) handle;
    const struct rb_root *tmr_tree = &tmrHandle->root;
    UtlTimerEvent *currEvent, *next;
    UtlTimestamp nowTms;
    UINT32 expires;

    utlLog_debug("dumping %d events", tmrHandle->numEvents);
    utlTm_get(&nowTms);

    rbtree_postorder_for_each_entry_safe(currEvent, next, tmr_tree, node)
    {
        /* this is the same code as in getTimeToNextEvent, integrate? */
        if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
        {
            /*
             * the currentevent is past due (nowTms is later than currEvent),
             * so expiry time is 0.
             */
            expires = 0;
        }
        else
        {
            /*
             * nowTms is earlier than currEvent, so currEvent is still in
             * the future.  
             */
            expires = utlTm_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
        }


        utlLog_debug("event %s expires in %ums (at %u.%03u) func=0x%x data=%p",
                currEvent->name,
                expires,
                currEvent->expireTms.sec,
                currEvent->expireTms.nsec/NSECS_IN_MSEC,
                currEvent->func,
                currEvent->ctxData);
    }
    return;
}

UtlRet utlTmr_replaceIfSooner(void *handle, UtlEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
    const UtlTimerHandle *tmrHandle = (const UtlTimerHandle *) handle;
    UtlTimerEvent *tmrEvent;
    UtlTimestamp nowTms;

    if ((tmrEvent = utlTmr_find(handle, func, ctxData)) != NULL)
    {
        /* find out the expire time of this event.  If it's sooner then the one in the 
         * timer list, then replace the one in list with this one.
         */
        utlTm_get(&nowTms);
        utlTm_addMilliSeconds(&nowTms, ms);
        if (IS_EARLIER_THAN(&nowTms, &(tmrEvent->expireTms)))
        {
            utlTmr_cancel((void*)tmrHandle, func, ctxData);
        }
        else
        {
            return UTLRET_SUCCESS;
        }
    } /* found */
    return(utlTmr_set(handle, func, ctxData, ms, name));
}

void utlTmr_cleanup(void **handle)
{
    UtlTimerHandle *tmrHandle = (UtlTimerHandle *) (*handle);
    struct rb_root *tmr_tree = &tmrHandle->root;

    if (!RB_EMPTY_ROOT(tmr_tree))
    {
        utlTmr_destory(handle, tmr_tree->rb_node);
    }

    UTLMEM_FREE_BUF_AND_NULL_PTR((*handle));

    return;
}
