#ifndef READER_HPP
#define READER_HPP

#include <vector>
#include <stdint.h>

/*
 * NOTE: std::uint8_t は C++11 から
 * stdint.h の uint8_t は C99 から
 */

class IReader {
public:
    virtual ~IReader();

    virtual ssize_t read(std::vector<uint8_t> &buf) = 0;
};

class BufferedReader : public IReader {
public:
    BufferedReader(IReader *reader);

    // buf.size() バイト読み込んで buf に保存する
    ssize_t read(std::vector<uint8_t> &buf);
    // n バイト読み飛ばす
    ssize_t discard(std::size_t n);
    // n バイト読み込んで buf に保存し、読んだデータは内部バッファに保持する
    ssize_t peek(std::vector<uint8_t> &buf, std::size_t n);
    // 1 バイト読み込む
    ssize_t readByte(uint8_t &buf);
    // delim が見つかるまで読み込む
    ssize_t readBytes(std::vector<uint8_t> &buf, uint8_t delim);

private:
    static const std::size_t kBufSize = 4096;

    IReader *reader_;
    uint8_t buf_[kBufSize];
};

#endif // READER_HPP
