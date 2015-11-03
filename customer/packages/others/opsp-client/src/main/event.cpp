#include "event.h"

bool CEvent::CheckEvent(void)
{
    if ((sAction == "none") || sAction == "") 
        return false;

    return true;
}
