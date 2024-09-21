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
    enum EventType {
        kReadEvent,
        kWriteEvent,
    };

    Event(EventType type, EventLoop &loop, Connection &conn, IEventHandler *handler);

    void handle();
    EventType getType() const;
    Connection &getConnection() const;
    void setEventHandler(IEventHandler *handler);

private:
    EventType type_;
    EventLoop &loop_;
    Connection &conn_;
    IEventHandler *handler_;
};

#endif // EVENT_HPP
