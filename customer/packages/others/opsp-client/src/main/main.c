#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libsol-util/utl.h>
#include <libsol-util/utl_logging.h>


int main(int argc, const char *argv[])
{
    utlLog_init();
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_error("log error test!");
    utlLog_cleanup();
    return 0;
}
