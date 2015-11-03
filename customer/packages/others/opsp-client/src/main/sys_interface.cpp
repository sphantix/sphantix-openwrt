#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <libsol-util/utl_logging.h>
#include "sys_interface.h"

void CSysInterface::Reboot(void)
{
    utlLog_debug("Reboot system.");
    system("reboot");
}
