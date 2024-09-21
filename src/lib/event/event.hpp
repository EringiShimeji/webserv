#ifndef EVENT_HPP
#define EVENT_HPP

#include "core/connection.hpp"
#include "core/event_loop.hpp"

class IEventHandler {
public:
    virtual ~IEventHandler();

    virtual void handleEvent(Event &ev) = 0;
};

class Event {
public:
    using EventType = enum {
        kReadEvent,
        kWriteEvent,
    };

    Event(EventLoop &loop, Connection &conn, IEventHandler *handler);

    void handle();
    EventType getType() const;
    Connection &getConnection() const;

private:
    IEventHandler *handler_;
};

#endif // EVENT_HPP
