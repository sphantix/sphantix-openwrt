#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <pthread.h>
#include "config.h"
#include "cmdopt.h"
#include "tsqueue.h"
#include "event.h"

class CClient{
    private:
        pthread_t tid_ws;
        pthread_t tid_handler;

    public:
        static volatile bool bKeepLooping;
        std::string sMac;
        std::string sKernelMD5;
        COption option;
        CConfig config;
        tsqueue<CEvent> event_queue;

    private:
        void GetMac(void);
        void GetKernelMD5(void);
        void InitLog(void);
        void InitWSServer(void);
        void InitHTTPServer(void);
        void InitSysInfo(void);
        void Init(void);
        void CleanUp(void);
        static void SigchldHandler(int exit_code);
        void InitSignals(void);

    public:
        CClient(): tid_ws(0), tid_handler(0){}
        static void TerminationHandler(int exit_code) { bKeepLooping = false; }
        void Run(void);
        pid_t Fork(void);
};

#endif /* __CLIENT_H__ */
