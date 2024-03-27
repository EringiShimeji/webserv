#include "task/read_request.hpp"
#include <fakeit.hpp>
#include <gtest/gtest.h>

using namespace fakeit;

TEST(ReadRequestOk, withBody) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));
    Fake(Method(stub_callback, trigger));

    // NOTE: 参照型の引数の検証が非対応なので, 変数に保存する
    // refs: https://github.com/eranpeer/FakeIt/issues/31
    Request req_set;
    When(Method(stub_context, setRequest)).Do([&](auto req) {
        req_set = req;
    });

    // body として hello を返す
    When(Method(stub_reader, read)).Do([](auto buf, auto) {
        std::memcpy(buf, "hello", 5);
        return Ok(5ul);
    });
    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("POST / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("Content-Length: 5\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("hello");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isOk());

    auto task_result = result.unwrap();
    EXPECT_EQ(task_result, kTaskComplete);

    // readLine が 4 回呼ばれる
    Verify(Method(stub_reader, readLine)).Exactly(3_Times);

    // read が 1 回以上呼ばれる
    Verify(Method(stub_reader, read)).AtLeastOnce();

    // パース結果
    auto req = Request(kMethodPost, "/", "HTTP/1.1", {{"Content-Length", "5"}}, "hello");
    EXPECT_EQ(req, req_set);
    // TODO: Fakeit が対応したらコメントアウトを外す
    // Verify(Method(stub_context, setRequest).Using(req)).Once();

    // コールバックが呼ばれているか
    Verify(Method(stub_callback, trigger)).Once();
}

TEST(ReadRequestOk, withoutBody) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));
    Fake(Method(stub_callback, trigger));

    Request req_set;
    When(Method(stub_context, setRequest)).Do([&](auto req) {
        req_set = req;
    });

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("GET / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("\r\n");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isOk());

    auto task_result = result.unwrap();
    EXPECT_EQ(task_result, kTaskComplete);

    Verify(Method(stub_reader, readLine)).Exactly(2_Times);
    Verify(Method(stub_reader, read)).Never();

    auto req = Request(kMethodGet, "/", "HTTP/1.1", {}, "");
    EXPECT_EQ(req, req_set);

    Verify(Method(stub_callback, trigger)).Once();
}

TEST(ReadRequestErr, readLineErr) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));
    Fake(Method(stub_callback, trigger));

    When(Method(stub_reader, readLine)).Do([](auto) {
        return Err<std::string>("readLine error");
    });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, readErr) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("POST / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("Content-Length: 5\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("\r\n");
            });
    When(Method(stub_reader, read)).Return(Err<std::string>("read error"));

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, parseErr) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("XXX / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("\r\n");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, requestLineEnd) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine)).Do([](auto) {
        return Ok<std::string>("GET / HTTP/1.1");
    });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, headerLineEnd) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("GET / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("Content-Length: 5");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, headerLineReadErr) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("GET / HTTP/1.1\r\n");
            })
            .Return(Err<std::string>("readLine error"));

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, parseHeaderFieldLineErr) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("GET / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("INVALID_HEADER\r\n");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}

TEST(ReadRequestErr, invalidContentLength) {
    Mock<IContext> stub_context;
    Mock<IReadRequestCallback> stub_callback;
    Mock<IBufferedReader> stub_reader;

    Fake(Method(stub_context, getManager),
         Method(stub_context, getClientFd));

    When(Method(stub_reader, readLine))
            .Do([](auto) {
                return Ok<std::string>("GET / HTTP/1.1\r\n");
            })
            .Do([](auto) {
                return Ok<std::string>("Content-Length: INVALID\r\n");
            });

    ReadRequest task(&stub_context.get(), &stub_callback.get(), &stub_reader.get());
    auto result = task.execute();
    ASSERT_TRUE(result.isErr());
}