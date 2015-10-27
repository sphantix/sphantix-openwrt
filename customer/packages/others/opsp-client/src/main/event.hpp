#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <string>

class event{
    public:
        int interval;
        std::string action;
    public:
        event(): interval(5), action("none") {}
        void event_handler(void);
};

#endif /* __EVENT_HPP__ */
