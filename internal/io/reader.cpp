#include "reader.hpp"
#include <cstring>
#include <unistd.h>

IReader::~IReader() {}

FdReader::FdReader(int fd, Ownership ownership) : fd_(fd), eof_(false), ownership_(ownership) {}

FdReader::~FdReader() {
    if (ownership_ == kOwnMove) {
        close(fd_);
    }
}

Result<std::size_t, std::string> FdReader::read(char *buf, const std::size_t n) {
    ssize_t bytes_read = ::read(fd_, buf, n);
    if (bytes_read == -1) {
        return Err(std::string(std::strerror(errno)));
    }
    if (bytes_read == 0) {
        eof_ = true;
    }
    return Ok(static_cast<std::size_t>(bytes_read));
}

bool FdReader::eof() const {
    return eof_;
}

BufferedReader::BufferedReader(IReader *reader, Ownership ownership)
    : reader_(reader), ownership_(ownership), buf_size_(kDefaultBufferSize), buf_read_pos_(0), buf_write_pos_(0) {
    buf_ = new char[buf_size_];
}

BufferedReader::BufferedReader(IReader *reader, std::size_t buffer_size, Ownership ownership)
    : reader_(reader), ownership_(ownership), buf_size_(buffer_size), buf_read_pos_(0), buf_write_pos_(0) {
    buf_ = new char[buf_size_];
}

BufferedReader::~BufferedReader() {
    delete[] buf_;
    if (ownership_ == kOwnMove) {
        delete reader_;
    }
}

Result<std::size_t, std::string> BufferedReader::read(char *buf, const std::size_t n) {
    std::size_t total_bytes_copied = 0;
    while (total_bytes_copied < n) {
        if (buf_read_pos_ == buf_write_pos_) {
            Result<std::size_t, std::string> fill_result = fillBuffer();
            if (fill_result.isErr()) {
                return Err(fill_result.unwrapErr());
            }
            if (fill_result.unwrap() == 0) {
                break;
            }
        }

        const std::size_t buf_bytes_left = buf_write_pos_ - buf_read_pos_;
        const std::size_t bytes_to_copy = std::min(n - total_bytes_copied, buf_bytes_left);
        const char *copy_src = buf_ + buf_read_pos_;
        char *copy_dst = buf + total_bytes_copied;
        std::memcpy(copy_dst, copy_src, bytes_to_copy);

        total_bytes_copied += bytes_to_copy;
        buf_read_pos_ += bytes_to_copy;
    }

    return Ok(total_bytes_copied);
}

Result<std::string, std::string> BufferedReader::readLine(const std::string &delimiter) {
    std::string line;
    while (true) {
        Result<std::string, std::string> read_line_result = readLineFromBuffer(delimiter);
        if (read_line_result.isErr()) {
            return Err(read_line_result.unwrapErr());
        }

        line += read_line_result.unwrap();
        if (utils::endsWith(line, delimiter) || eof()) {
            break;
        }

        Result<std::size_t, std::string> fill_result = fillBuffer();
        if (fill_result.isErr()) {
            return Err(fill_result.unwrapErr());
        }
    }

    return Ok(line);
}

bool BufferedReader::eof() const {
    return reader_->eof() && buf_read_pos_ == buf_write_pos_;
}

Result<std::size_t, std::string> BufferedReader::fillBuffer() {
    Result<std::size_t, std::string> read_result = reader_->read(buf_, buf_size_);
    if (read_result.isErr()) {
        return Err(read_result.unwrapErr());
    }
    buf_write_pos_ = read_result.unwrap();
    buf_read_pos_ = 0;
    return Ok(buf_write_pos_);
}

Result<std::string, std::string> BufferedReader::readLineFromBuffer(const std::string &delimiter) {
    if (buf_read_pos_ >= buf_write_pos_) {
        return Ok<std::string>("");
    }

    const char *line_start = buf_ + buf_read_pos_;
    const std::size_t buf_bytes_left = buf_write_pos_ - buf_read_pos_;
    const Option<char *> search_result = utils::strnstr(line_start, delimiter.c_str(), buf_bytes_left);
    if (search_result.isNone()) {
        // If the delimiter is not found, return the entire buffer
        const std::string line(line_start, buf_bytes_left);
        buf_read_pos_ = buf_write_pos_;
        return Ok(line);
    }

    // If the delimiter is found, return the line up to the delimiter
    const char *line_end = search_result.unwrap();
    const std::size_t line_length = line_end - line_start + delimiter.size();
    std::string line(line_start, line_length);
    buf_read_pos_ += line_length;
    return Ok(line);
}
