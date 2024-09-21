#ifndef EVENT_ACTIONS_HPP
#define EVENT_ACTIONS_HPP

#include "event.hpp"
#include "core/event_loop.hpp"

class IEventActions {
public:
    virtual ~IEventActions();

    virtual int addEvent(Event &ev) = 0;
    virtual int deleteEvent(Event &ev) = 0;
    virtual int processEvents(EventLoop &loop) = 0;
};

#endif // EVENT_ACTIONS_HPP
