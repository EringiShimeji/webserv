#include "io/reader.hpp"
#include <gtest/gtest.h>

class FdReaderTest : public ::testing::Test {
protected:
    int fd_ = -1;
    char tmpfile_[32] = "/tmp/fd_reader_test";
    static constexpr char kFileContent[] = "Hello, world!\n";
    static constexpr std::size_t kFileContentLen = 14;

    void SetUp() override {
        fd_ = mkstemp(tmpfile_);
        ASSERT_NE(fd_, -1);
        ASSERT_EQ(write(fd_, kFileContent, kFileContentLen), kFileContentLen);
        ASSERT_EQ(lseek(fd_, 0, SEEK_SET), 0);
    }

    void TearDown() override {
        close(fd_);
        unlink(tmpfile_);
    }
};

void expectBufferEq(const char *buf, const char *expected, std::size_t n) {
    EXPECT_EQ(memcmp(buf, expected, n), 0);
}

TEST_F(FdReaderTest, read) {
    FdReader reader(fd_);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, kFileContentLen);
    expectBufferEq(buf, kFileContent, len);
}

TEST_F(FdReaderTest, readMove) {
    FdReader reader(fd_, kOwnMove);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, kFileContentLen);
    expectBufferEq(buf, kFileContent, len);
}

TEST_F(FdReaderTest, readPartial) {
    FdReader reader(fd_);
    char buf[8];
    auto result = reader.read(buf, 8);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 8);
    expectBufferEq(buf, kFileContent, len);
}

TEST_F(FdReaderTest, readZero) {
    FdReader reader(fd_);
    char buf[16];
    auto result = reader.read(buf, 0);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 0);
}

TEST_F(FdReaderTest, readTooMuch) {
    FdReader reader(fd_);
    char buf[16];
    auto result = reader.read(buf, 32);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, kFileContentLen);
    expectBufferEq(buf, kFileContent, len);
}

TEST_F(FdReaderTest, eof) {
    FdReader reader(fd_);
    char buf[16];
    reader.read(buf, 16);
    reader.read(buf, 16);

    EXPECT_TRUE(reader.eof());
}

TEST_F(FdReaderTest, readError) {
    FdReader reader(fd_);
    close(fd_);
    char buf[16];
    auto result = reader.read(buf, 16);

    EXPECT_TRUE(result.isErr());
}