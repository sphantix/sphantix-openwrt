#include <pthread.h>
#include <string>
#include <libsol-util/utl_logging.h>
#include "client.h"
#include "cJSON.h"
#include "event.h"
#include "handler_thread.h"
#include "http_interface.h"

extern CClient client;

static void handle_event(const CEvent &event)
{
    CHttpInterface http_handler;

    utlLog_debug("action: %s", event.sAction.c_str());

    if (event.sAction == "plugin") 
    {
        http_handler.plugin.UpdatePlugins(client.sMac, client.sKernelMD5, client.sHttpServerFullPath);
    }
    else if (event.sAction == "command")
    {
        if (event.sData == "reboot") 
            http_handler.sysintf.Reboot();
        if (event.sData == "shell") 
            http_handler.sysintf.ReboundShell(client.sMac);
    }
}

void *thread_handler(void *arg)
{
    CEvent ev;

    utlLog_debug("start handler thread");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);   
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(client.bKeepLooping){
        client.event_queue.wait_and_pop(ev);
        handle_event(ev);
    }
    return NULL;
}
