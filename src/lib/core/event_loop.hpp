#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <vector>
#include "connection.hpp"
#include "event/event_actions.hpp"

class EventLoop {
public:
    EventLoop();

    void run();
    int addConnection(Connection *conn);
    int deleteConnection(Connection *conn);

private:
    static const std::size_t kMaxConnections = 1024;

    Connection connections_[kMaxConnections];
    IEventActions *eventActions_;
};

#endif // EVENT_LOOP_HPP
