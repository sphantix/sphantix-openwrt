#include <libsol-util/utl_logging.h>
#include "event.hpp"

void event::event_handler(void)
{
    utlLog_debug("handle action: %s", action.c_str());

}
