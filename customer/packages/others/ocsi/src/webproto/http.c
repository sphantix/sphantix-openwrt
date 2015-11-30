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

#include <libsol-util/utl_memory.h>
#include "http.h"

sHttpHdrs *http_new_http_hdrs()
{
   return ((sHttpHdrs *)utlMem_alloc(sizeof(sHttpHdrs), ALLOC_ZEROIZE));

}

void http_free_http_hdrs(sHttpHdrs *p)
{
   sCookieHdr   *cp, *last;
   utlMem_free(p->content_type);
   utlMem_free(p->protocol);
   utlMem_free(p->www_authenticate);
   utlMem_free(p->authorization);
   utlMem_free(p->tansfer_encoding);
   utlMem_free(p->connection);
   utlMem_free(p->method);
   utlMem_free(p->path);
   utlMem_free(p->host);
   cp = p->setCookies;
   while (cp)
   {
      last = cp->next;
      utlMem_free(cp->name);
      utlMem_free(cp->value);
      utlMem_free(cp);
      cp = last;
   }
   utlMem_free(p->message);
   utlMem_free(p->location_hdr);
   utlMem_free(p->file_name);
   utlMem_free(p->arg);
   utlMem_free(p);
}


