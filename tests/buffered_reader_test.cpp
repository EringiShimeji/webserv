#include "io/reader.hpp"
#include <fakeit.hpp>
#include <gtest/gtest.h>

using namespace fakeit;

// デフォルトのバッファは大きいので, 極力小さい値を使う
constexpr std::size_t kBufferSize = 64;

// "a" を読み続ける Reader
class BufferedReaderInfinite : public ::testing::Test {
protected:
    Mock<IReader> stub_;

    void SetUp() override {
        When(Method(stub_, eof)).AlwaysReturn(false);
        When(Method(stub_, read)).AlwaysDo([](auto buf, auto n) {
            std::string s(n, 'a');
            std::memcpy(buf, s.c_str(), n);
            return Ok(n);
        });
    }

    // buf が期待通りの文字列で埋まっているかを検証する
    static void expectBufferEq(const char *buf, std::size_t n) {
        for (std::size_t i = 0; i < n; i++) {
            EXPECT_EQ(buf[i], 'a');
        }
    }
};

TEST_F(BufferedReaderInfinite, read) {
    BufferedReader reader(&stub_.get(), kBufferSize);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 16);
    expectBufferEq(buf, len);
}

TEST_F(BufferedReaderInfinite, readMove) {
    BufferedReader reader(&stub_.get(), kBufferSize);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 16);
    expectBufferEq(buf, len);
}

TEST_F(BufferedReaderInfinite, readMoreThanBuffer) {
    BufferedReader reader(&stub_.get(), kBufferSize);
    char buf[kBufferSize * 2];
    auto result = reader.read(buf, kBufferSize * 2);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, kBufferSize * 2);
    expectBufferEq(buf, kBufferSize);
}

TEST_F(BufferedReaderInfinite, smallBuffer) {
    BufferedReader reader(&stub_.get(), 1);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 16);
    expectBufferEq(buf, len);
}

// 連続で read
TEST_F(BufferedReaderInfinite, readTwice) {
    BufferedReader reader(&stub_.get());
    for (int i = 0; i < 2; i++) {
        char buf[16];
        auto result = reader.read(buf, 16);
        ASSERT_TRUE(result.isOk());

        auto len = result.unwrap();
        EXPECT_EQ(len, 16);
        expectBufferEq(buf, len);
    }
}

// バッファリングされていることを確認
TEST_F(BufferedReaderInfinite, buffering) {
    BufferedReader reader(&stub_.get(), kBufferSize);
    for (int i = 0; i < 2; i++) {
        char buf[16];
        auto result = reader.read(buf, 16);
        ASSERT_TRUE(result.isOk());
    }

    // スタブの read は 1 回しか呼ばれていないことを確認
    Verify(Method(stub_, read)).Once();
}

TEST_F(BufferedReaderInfinite, bufferingRefill) {
    BufferedReader reader(&stub_.get(), 8);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    // スタブの read は 2 回呼ばれていることを確認
    Verify(Method(stub_, read)).Exactly(2_Times);
}

// EOF に到達した場合
TEST(BufferedReaderOk, eof) {
    Mock<IReader> stub;
    // stub の eof を呼ぶかは実装依存なので, read をうまくスタブする
    Fake(Method(stub, eof));
    When(Method(stub, read))
            .Do([](auto buf, auto) {
                std::memcpy(buf, "hello", 5);
                return Ok(5ul);
            })
            .Return(Ok(0ul));

    BufferedReader reader(&stub.get(), kBufferSize);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isOk());

    auto len = result.unwrap();
    EXPECT_EQ(len, 5);
    EXPECT_EQ(std::memcmp(buf, "hello", 5), 0);
}

TEST(BufferedReaderErr, readErr) {
    Mock<IReader> stub;
    Fake(Method(stub, eof));
    When(Method(stub, read)).Return(Err<std::string>("read error"));

    BufferedReader reader(&stub.get(), kBufferSize);
    char buf[16];
    auto result = reader.read(buf, 16);
    ASSERT_TRUE(result.isErr());
}

TEST(BufferedReaderReadLineOk, normal) {
    Mock<IReader> stub;
    Fake(Method(stub, eof));
    // 2 回目の read で行末に到達
    When(Method(stub, read))
            .Do([](auto buf, auto) {
                std::memcpy(buf, "123", 3);
                return Ok(3ul);
            })
            .Do([](auto buf, auto) {
                std::memcpy(buf, "456\n789\n", 8);
                return Ok(8ul);
            })
            .Return(Ok(0ul));

    BufferedReader reader(&stub.get(), kBufferSize);
    auto result = reader.readLine("\n");
    ASSERT_TRUE(result.isOk());
    EXPECT_EQ(result.unwrap(), "123456\n");

    result = reader.readLine("\n");
    ASSERT_TRUE(result.isOk());
    EXPECT_EQ(result.unwrap(), "789\n");
}

// \r\n 区切り
TEST(BufferedReaderReadLineOk, crlf) {
    Mock<IReader> stub;
    Fake(Method(stub, eof));
    // 2 回目の read で行末に到達
    When(Method(stub, read))
            .Do([](auto buf, auto) {
                std::memcpy(buf, "123", 3);
                return Ok(3ul);
            })
            .Do([](auto buf, auto) {
                std::memcpy(buf, "456\r\n789\r\n", 10);
                return Ok(10ul);
            })
            .Return(Ok(0ul));

    BufferedReader reader(&stub.get(), kBufferSize);
    auto result = reader.readLine("\r\n");
    ASSERT_TRUE(result.isOk());
    EXPECT_EQ(result.unwrap(), "123456\r\n");

    result = reader.readLine("\r\n");
    ASSERT_TRUE(result.isOk());
    EXPECT_EQ(result.unwrap(), "789\r\n");
}

// バッファリングされていることを確認
TEST(BufferedReaderReadLineOk, buffering) {
    Mock<IReader> stub;
    Fake(Method(stub, eof));
    // 2 回目の read で行末に到達
    When(Method(stub, read)).Do([](auto buf, auto) {
        std::memcpy(buf, "123\n456\n", 8);
        return Ok(8ul);
    });

    BufferedReader reader(&stub.get(), kBufferSize);
    auto result = reader.readLine("\n");
    ASSERT_TRUE(result.isOk());
    result = reader.readLine("\n");
    ASSERT_TRUE(result.isOk());

    // スタブの read は 1 回しか呼ばれていないことを確認
    Verify(Method(stub, read)).Exactly(1_Times);
}

// TODO: 改行で終わらない場合の readLine のテスト
// eof がいつ true を返すようにスタブするべきかが実装依存

TEST(BufferedReaderReadLineErr, readErr) {
    Mock<IReader> stub;
    Fake(Method(stub, eof));
    When(Method(stub, read)).Return(Err<std::string>("read error"));

    BufferedReader reader(&stub.get(), kBufferSize);
    auto result = reader.readLine("\n");
    ASSERT_TRUE(result.isErr());
}

class BufferedReaderFile : public ::testing::Test {
protected:
    int fd_ = -1;
    char tmpfile_[32] = "/tmp/buffered_reader_test";
    static constexpr char kFileContent[] = "Hello, world!\n";

    void SetUp() override {
        fd_ = mkstemp(tmpfile_);
        ASSERT_NE(fd_, -1);
        ASSERT_EQ(write(fd_, kFileContent, sizeof(kFileContent)), sizeof(kFileContent));
        ASSERT_EQ(lseek(fd_, 0, SEEK_SET), 0);
    }

    void TearDown() override {
        close(fd_);
        unlink(tmpfile_);
    }
};

TEST_F(BufferedReaderFile, read) {
    // デストラクタで fd, reader を破棄
    BufferedReader buffered_reader(new FdReader(fd_, kOwnMove), kOwnMove);
    char buf[16];
    auto result = buffered_reader.readLine("\n");
    ASSERT_TRUE(result.isOk());

    EXPECT_EQ(result.unwrap(), "Hello, world!\n");
}