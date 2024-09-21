#ifndef WRITER_HPP
#define WRITER_HPP

#include <vector>
#include <stdint.h>

class IWriter {
public:
    virtual ~IWriter();

    virtual ssize_t write(const std::vector<uint8_t> &buf) = 0;
};

#endif // WRITER_HPP
