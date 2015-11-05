#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <vector>
#include <pthread.h>
#include "config.h"
#include "cmdopt.h"
#include "tsqueue.h"
#include "event.h"

class CClient{
    private:
        pthread_t tid_ws;
        pthread_t tid_handler;
        std::string sClientInitFlagFile;
        std::vector<std::string> vServerList;

    public:
        static volatile bool bKeepLooping;
        std::string sMac;
        std::string sKernelMD5;
        std::string sServerUrl;
        std::string sWSServerFullPath;
        std::string sHttpServerFullPath;
        COption option;
        CConfig config;
        tsqueue<CEvent> event_queue;

    private:
        void GetMac(void);
        void GetKernelMD5(void);
        void InitLog(void);
        std::vector<std::string> Split(const std::string str, const std::string pattern);
        void InitWSServer(void);
        void InitHTTPServer(void);
        void InitServerInfo(void);
        void InitSysInfo(void);
        void Init(void);
        void CleanUp(void);
        bool IsFisrtBoot(void);
        void FirstSyncWithServer(void);
        static void SigchldHandler(int exit_code);
        void InitSignals(void);

    public:
        CClient(): tid_ws(0), tid_handler(0), sClientInitFlagFile("/etc/ClientInitFlag"){}
        static void TerminationHandler(int exit_code) { bKeepLooping = false; }
        void Run(void);
        pid_t Fork(void);
        void RandomSelectServerUrl(void);
};

#endif /* __CLIENT_H__ */
