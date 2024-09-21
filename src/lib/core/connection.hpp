#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include "utils/io/read_writer.hpp"

class Event;

class Connection : public IReadWriter {
public:
    Connection(int fd);

    ssize_t read(std::vector<uint8_t> &buf);
    ssize_t write(const std::vector<uint8_t> &buf);
    void close();
    int getFd();
    Event &getReadEvent();
    Event &getWriteEvent();

private:
    FileReadWriter readWriter_;
    Event *readEvent_;
    Event *writeEvent_;
};

#endif // CONNECTION_HPP