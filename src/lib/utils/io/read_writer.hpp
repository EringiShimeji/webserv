#ifndef READ_WRITER_HPP
#define READ_WRITER_HPP

#include "reader.hpp"
#include "writer.hpp"

class IReadWriter : public IReader, public IWriter {
public:
    virtual ~IReadWriter();
};

class FileReadWriter : public IReadWriter {
public:
    FileReadWriter(int fd);

    ssize_t read(std::vector<uint8_t> &buf);
    ssize_t write(const std::vector<uint8_t> &buf);
    void close();
    int getFd();

private:
    int fd_;
};

#endif // READ_WRITER_HPP
