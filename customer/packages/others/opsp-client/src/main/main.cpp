#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libsol-util/utl_logging.h>
#include <libsol-util/utl_strconv.h>
#include <libsol-util/utl_memory.h>
#include <libsol-util/utl_ini_parser.h>
#include "config.hpp"
#include "cmdopt.hpp"
#include "easywsclient.hpp"
#include "cJSON.hpp"
#include "event.hpp"

using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;
static event ev;

static void opsp_client_init(config *cfg)
{
    utlLog_init();
    utlLog_setLevel((UtlLogLevel)cfg->log_level);
    if (cfg->log_dest == "file") 
    {
        utlLog_setDestination(LOG_DEST_FILE);
        cfg->log_file = cfg->log_path + cfg->log_name;
        utlLog_setLogFile((char *)cfg->log_file.c_str());
    }
    else if (cfg->log_dest == "stderr")
    {
        utlLog_setDestination(LOG_DEST_STDERR);
    }

    cfg->server_full_path = cfg->server_url + cfg->server_path;
}


static void opsp_client_cleanup(void)
{
    utlLog_cleanup();
}

void handle_message(const std::string &message)
{
    utlLog_debug("recv: %s", message.c_str());
    cJSON *root = cJSON_Parse(message.c_str());

    if(root)
    {
        ev.action = cJSON_GetObjectItem(root,"action")->valuestring;
        ev.interval = cJSON_GetObjectItem(root,"interval")->valueint;
        ev.event_handler();

        utlLog_debug("action = %s, interval = %d", ev.action.c_str(), ev.interval);
    }
}

int main(int argc, char **argv)
{
    option op;
    config cfg;

    //parse command
    op.parse_cmd(argc, argv);
    //read & parse config file
    cfg.config_parse(op.configfile);
    //client init
    opsp_client_init(&cfg);

    while(1){
        do{
            sleep(10);
            ws = WebSocket::from_url(cfg.server_full_path);
        }while(ws == NULL);

        utlLog_debug("create ws");
        ws->send("111111111112");
        ws->send("0001");
        while (ws->getReadyState() != WebSocket::CLOSED) {
            ws->poll(1000);
            ws->dispatch(handle_message);
            sleep(ev.interval);
            ws->send("{\"action\": \"get\"}");
        }
        delete ws;
        ws = NULL;
    }

    //client cleanup before exit
    opsp_client_cleanup();

    return 0;
}
