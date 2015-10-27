#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cmdopt.hpp"

static const char *options = "dc:v";
static const char* version = "1.0.1";

int option::parser_cmd(int argc, char **argv){
    int opt = 0;
    opt = getopt(argc, argv, options);
    while( opt != -1 ){
        switch( opt ){
            case 'd':
                daemon = true;
                break;
            case 'c':
                config = optarg;
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
