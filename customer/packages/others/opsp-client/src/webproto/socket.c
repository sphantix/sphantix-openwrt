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

/* blocking read */
int tcp_read_wait(sock_ctx *pc, char *ptr, int nbytes)
{
    int nread=0;
    int flags, bflags;

    /* turn on synchroneous I/O, this call will block. */
    {
        flags = (long) fcntl(pc->fd, F_GETFL);
        bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
        fcntl(pc->fd, F_SETFL, bflags);
    }

    errno = 0;
    switch (pc->type) {
        case iNormal:
            nread = read(pc->fd, ptr, nbytes);
            break;
#ifdef USE_SSL
        case iSsl:
#ifdef DEBUGSSL
            utlLog_debug("%s read_SSL(%d, lth=%d)", getticks(),pc->fd, nbytes);
            utlLog_debug(" result=%d", nread = SSL_read(pc->ssl, (void *) ptr, nbytes));
#else
            nread = SSL_read(pc->ssl, (void *) ptr, nbytes);
#endif
            break;
#endif
        default:
            utlLog_error("Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
            break;
    }
    if (nread > nbytes) {
        utlLog_error("proto_READ of %d returned %d", nbytes, nread);
    }

    fcntl(pc->fd, F_SETFL, flags); /* remove blocking flags */

    return nread;
}

int read_timeout(int socket, int time_out_sec)
{
    fd_set readSet;

    FD_ZERO(&readSet);
    FD_SET(socket_, &readSet);
    if (time_out_sec == 0)
    {
        // zero means BLOCKING operation (will wait indefinitely)
        return (select(socket_ + 1, &readSet, NULL, NULL, NULL));
    }
    // otherwise, wait up to the specified time period
    struct timeval tv;

    tv.tv_sec = time_out_sec;
    tv.tv_usec = 0;

    return (select(socket_ + 1, &readSet, NULL, NULL, &tv));

    // returns 0 if the time limit expired.
    // returns -1 on error, otherwise there is data on the port ready to read
}

int tcp_readn(sock_ctx *sc, char *ptr, int nbytes)
{
    int nleft, nread=0;
    int errnoval;
    char *pReadBuf = ptr;
    int readBytes = 0;

    nleft = nbytes;
    while (nleft > 0) {
        errno =0;
        switch (pc->type) {
            case iNormal:
                if (read_timeout(pc->fd, 30) <= 0) 
                {
                    utlLog_error("read packet timeout");
                    return -99; //timeout!!!
                }                   

                nread = read(pc->fd, ptr, nleft);
                break;
#ifdef USE_SSL
            case iSsl:
#ifdef DEBUGSSL
                utlLog_debug("%s SSL_read(%d, lth=%d)", getticks(),pc->fd, nleft);
                nread = SSL_read(pc->ssl, (void *) ptr, nleft);
                utlLog_debug(" result=%d", nread);
#else
                nread = SSL_read(pc->ssl, (void *) ptr, nleft);
#endif
                break;
#endif
            default:
                utlLog_error("Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
                break;
        }

        if (nread < 0) {                            /* This function will read until the byte cnt*/
            errnoval=errno;                         /* is reached or the return is <0. In the case*/
            if (errnoval==EAGAIN )                  /* of non-blocking reads this may happen after*/
                return nbytes-nleft;                /* some bytes have been retrieved. The EAGAIN*/
            else                                    /* status indicates that more are coming */
                /* Other possibilites are ECONNRESET indicating*/
                /* that the tcp connection is broken */
                fprintf(stderr,"!!!!!!!! read(fd=%d) error=%d\n", pc->fd, errnoval);
            return nread; /* error, return < 0 */

        } else if (nread == 0) {
            break; /* EOF */
        }

        nleft -= nread;
        ptr += nread;
    }

    readBytes =  nbytes - nleft;

    return readBytes; /* return >= 0 */    
}

/*
 * Return number of bytes written or -1.
 * If -1 check for errno for EAGAIN and recall.
 *----------------------------------------------------------------------*/
int tcp_writen(sock_ctx *pc, const char *ptr, int nbytes)
{
   int  nwritten=0;
   
   errno = 0;
   switch (pc->type)
   {
      case iNormal:
         nwritten = write(pc->fd, ptr, nbytes);
         break;
#ifdef USE_SSL
      case iSsl:
#ifdef DEBUGSSL
         utlLog_debug("%s SSL_write(%d, lth=%d)", getticks(),pc->fd, nbytes);
         nwritten = SSL_write(pc->ssl, ptr, nbytes);
         utlLog_debug("result=%d", nwritten);
#else
         nwritten = SSL_write(pc->ssl, ptr, nbytes);
#endif
         break;
#endif
      default:
         utlLog_error("Impossible error; writen() illegal ProtoCtx type (%d)", pc->type);
         break;
   }

   if (nwritten <= 0)
   {
      if (errno!=EAGAIN)
            return nwritten;
   }

   return nwritten;
}  /* End of tcp_writen() */

void tcp_printline(sock_ctx *pc, const char *fmt, ...)
{
    char *p;
    va_list ap;
    int n;
    int size;

    size = 1024;
    if ((p = utlMem_alloc(size, 0)) == NULL) {
        utlLog_error("failed to malloc(%d)", size);
        return;
    }

    while (1) {
        /* try to print in the allocated space */
        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);

        if (n < 0) {
            utlLog_notice("fdprintf() vsnprintf failed *%d): %s (%d) fmt=\"%s\"", n, strerror(errno), errno, fmt);
            return;
        } else if (n >= 0 && n < size) {
            /* print succeeded, let's write it on outstream */
            tcp_writen(pc, p, n);
            utlMem_free(p);
            return;
        } else {
            utlLog_debug("vsnprintf, only wrote %d bytes, retrying: fmt=\"%s\" strlen(fmt)=%d size=%d",
                    n, fmt, strlen(fmt), size);
            size *= 2;
            if ((p = utlMem_realloc(p, size)) == NULL) {
                utlLog_error("failed to realloc(%d)", size);
                return;
            }
        }
    }/*end while(1)*/
}
