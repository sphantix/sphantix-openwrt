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

#ifndef __HTTP_H__
#define __HTTP_H__

typedef unsigned int uiIpAddr;

typedef struct cookie_hdr {
    struct  cookie_hdr *next;
    char	*name;
    char	*value;
} sCookieHdr;

typedef struct {
    /* common */
    char *content_type;
    char *protocol;
    char *www_authenticate;
    char *authorization;
    char *tansfer_encoding;
    char *connection;
    /* request */
    char *method;
    char *path;
    char *host;
    int  port;
    int  content_length;

    /* result */
    int  status_code;
    sCookieHdr	*set_cookies;
    char *message;
    char *location_hdr;		/* from 3xx status response */

    /* request derived */
    uiIpAddr addr;  /* IP-address of communicating entity */
    char *file_name;
    char *arg;
} sHttpHdrs;

#endif /* __HTTP_H__ */ 
