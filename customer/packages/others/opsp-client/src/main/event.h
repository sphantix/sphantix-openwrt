#ifndef __EVENT_H__
#define __EVENT_H__

#include <string>

class CEvent{
    public:
        std::string sAction;
        std::string sData;
        std::string sMessageId;
    public:
        CEvent(): sAction("none"), sData(""), sMessageId(""){}
        bool CheckEvent(void);
};

#endif /* __EVENT_H__ */
