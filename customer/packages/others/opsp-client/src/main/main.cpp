#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <libsol-util/utl_logging.h>
#include "main.h"
#include "client.h"
#include "ws_thread.h"
#include "handler_thread.h"

CClient client; 

volatile bool CClient::bKeepLooping = true;

pid_t safe_fork(void)
{
    pid_t result;
    result = fork();

    if (result == -1) {
        fprintf(stderr, "Failed to fork: %s.  Bailing out\n", strerror(errno));
        exit(E_FORKERROR);
    }

    return result;
}

void sigchld_handler(int exit_code)
{
    int status;
    pid_t pid;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        utlLog_debug("Handler for SIGCHLD reaped child PID %d", pid);
    }
}

static void termination_handler(int exit_code)
{
    CClient::bKeepLooping = false;
}

void init_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(): %s", strerror(errno));
        exit(E_SETSIGNALERR);
    }

    /* Trap SIGPIPE */
    /* This is done so that when libhttpd does a socket operation on
     * a disconnected socket (i.e.: Broken Pipes) we catch the signal
     * and do nothing. The alternative is to exit. SIGPIPE are harmless
     * if not desirable.
     */
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(): %s", strerror(errno));
        exit(E_SETSIGNALERR);
    }

    sa.sa_handler = termination_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    /* Trap SIGTERM */
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(): %s", strerror(errno));
        exit(E_SETSIGNALERR);
    }

    /* Trap SIGQUIT */
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(): %s", strerror(errno));
        exit(E_SETSIGNALERR);
    }

    /* Trap SIGINT */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(): %s", strerror(errno));
        exit(E_SETSIGNALERR);
    }
}

int main(int argc, char **argv)
{
    //parse command
    client.option.ParseCmd(argc, argv);

    //init signal handlers
    init_signals();

    if (client.option.bIsDaemon)
    {
        switch (safe_fork())
        {
            case 0:                /* child */
                setsid();
                client.Run();
                break;

            default:               /* parent */
                exit(E_SUCCESS);
                break;
        }
    }
    else
    {
        client.Run();
    }

    return 0;
}
