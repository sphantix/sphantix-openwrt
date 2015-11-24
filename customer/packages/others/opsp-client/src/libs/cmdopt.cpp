#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cmdopt.h"

static const char *options = "dc:v";
static const char* version = "1.0.1";

int COption::ParseCmd(int argc, char **argv)
{
    int opt = 0;
    opt = getopt(argc, argv, options);
    while( opt != -1 ){
        switch( opt ){
            case 'd':
                bIsDaemon = true;
                break;
            case 'c':
                sConfigFile = optarg;
                break;
            case 'v':
                printf("%s\r\n", version);
                break;
            default:
                break;
        }
        opt = getopt(argc, argv, options);
    }
    return 0;
}
