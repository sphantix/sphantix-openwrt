#include <pthread.h>
#include <libsol-util/utl_logging.h>
#include "client.h"
#include "easywsclient.h"
#include "cJSON.h"
#include "event.h"
#include "ws_thread.h"
#include "main.h"

extern CClient client;

static int interval = 10;
using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

static void handle_message(const std::string &message)
{
    utlLog_debug("recv: %s", message.c_str());
    cJSON *root = cJSON_Parse(message.c_str());

    if(root)
    {
        CEvent ev;
        cJSON *pAction = cJSON_GetObjectItem(root,"action");
        cJSON *pData = cJSON_GetObjectItem(root,"data");
        cJSON *pMessageId = cJSON_GetObjectItem(root,"message_id");
        cJSON *pInterval = cJSON_GetObjectItem(root,"interval");

        if (pAction != NULL) 
            ev.sAction = pAction->valuestring;
        if (pData != NULL) 
            ev.sData = pData->valuestring;
        if (pMessageId != NULL) 
            ev.sMessageId = pMessageId->valuestring;
        if (pInterval != NULL)
            interval = pInterval->valueint;

        cJSON_Delete(root);

        if(ev.CheckEvent())
            client.event_queue.push(ev);
    }
}

void *thread_websocket(void *arg)
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
    struct timespec timeout;
    std::string server_full_path = (char *)arg;

    utlLog_debug("start websocket thread");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);   
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(client.bKeepLooping)
    {
        do{
            sleep(10);
            ws = WebSocket::from_url(server_full_path);
        }while(ws == NULL);

        ws->send(client.sMac);
        ws->send(OPSP_CLIENT_VERSION);
        while (ws->getReadyState() != WebSocket::CLOSED) {
            ws->poll(1000);
            ws->dispatch(handle_message);

            /* Sleep for ev.interval seconds... */
            timeout.tv_sec = time(NULL) + interval;
            timeout.tv_nsec = 0;

            /* Mutex must be locked for pthread_cond_timedwait... */
            pthread_mutex_lock(&cond_mutex);

            /* Thread safe "sleep" */
            pthread_cond_timedwait(&cond, &cond_mutex, &timeout);

            /* No longer needs to be locked */
            pthread_mutex_unlock(&cond_mutex);

            ws->send("{\"action\": \"get\"}");
        }
    }
    delete ws;
    ws = NULL;

    return NULL;
}
