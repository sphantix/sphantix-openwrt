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

#ifndef __SOCKET_H__
#define __SOCKET_H__

typedef void (*fHandler)(void *, int lth);

typedef enum {
    eTCP,
    eUDP 
} eSockType;

typedef enum {
    eUnknown,
    eNormal,
    eLast
} ePostCtxType;

typedef struct {
    eSockType      sock_type;
    ePostCtxType   post_type;
    int            fd;
    /* internal use */
    handler        cb;
    void           *data;
} sSockCtx;

#endif /* __SOCKET_H__ */
