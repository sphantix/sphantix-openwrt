/*
 * File Name: pty.h
 * Author: Sphantix
 * Mail: sphantix@gmail.com
 * Created Time: Fri 08 Jan 2016 10:28:17 AM CST
 */

#ifndef __CPTY_H__
#define __CPTY_H__

#include <cstddef>

class CPty {
    public:
        static void Spawn(int sock);
    private:
        static ssize_t Writen(int fd, const void *ptr, size_t n);
        static void SetRaw(int fd);
        static void Loop(int pty_fd);
};

#endif /* __CPTY_H__ */
