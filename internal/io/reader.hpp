#ifndef READER_HPP
#define READER_HPP

#include "utils/ownership.hpp"
#include "utils/result.hpp"
#include "utils/utils.hpp"
#include <cerrno>
#include <string>

class IReader {
public:
    virtual ~IReader();
    // Read up to n bytes into buf
    // Return the number of bytes read, or an error if one occurred
    virtual Result<std::size_t, std::string> read(char *buf, std::size_t n) = 0;
    virtual bool eof() const = 0;
};

class FdReader : public IReader {
public:
    explicit FdReader(int fd, Ownership ownership = kOwnBorrow);
    // Close the file descriptor if ownership is kOwn
    virtual ~FdReader();

    virtual Result<std::size_t, std::string> read(char *buf, std::size_t n);
    virtual bool eof() const;

private:
    int fd_;
    bool eof_;
    Ownership ownership_;
};

class IBufferedReader : public IReader {
public:
    virtual Result<std::string, std::string> readLine(const std::string &delimiter) = 0;
};

class BufferedReader : public IBufferedReader {
public:
    explicit BufferedReader(IReader *reader, Ownership ownership = kOwnBorrow);
    explicit BufferedReader(IReader *reader, std::size_t buffer_size, Ownership ownership = kOwnBorrow);
    // Delete the reader if ownership is kOwn
    virtual ~BufferedReader();

    virtual Result<std::size_t, std::string> read(char *buf, std::size_t n);
    virtual Result<std::string, std::string> readLine(const std::string &delimiter);
    bool eof() const;

private:
    static const std::size_t kDefaultBufferSize = 4 * utils::kKiB;
    IReader *reader_;
    Ownership ownership_;
    char *buf_;
    std::size_t buf_size_;
    std::size_t buf_read_pos_;
    std::size_t buf_write_pos_;

    Result<std::size_t, std::string> fillBuffer();
    Result<std::string, std::string> readLineFromBuffer(const std::string &delimiter);
};

#endif //READER_HPP
