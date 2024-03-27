#include "http/context.hpp"
#include <gtest/gtest.h>

class ContextTest : public ::testing::Test {
protected:
    IOTaskManager manager_;
    const int client_fd_ = 0;
    Context context_ = Context(manager_, client_fd_);
    Request request_ = Request(kMethodPost, "/path", "HTTP/1.1", {}, "body");

    void SetUp() override {
        context_.setRequest(request_);
    }
};

TEST_F(ContextTest, getRequest) {
    EXPECT_EQ(context_.getRequest(), request_);
}

TEST_F(ContextTest, setRequest) {
    Request new_request = Request(kMethodGet, "/new_path", "HTTP/1.0", {{"key", "value"}}, "");
    context_.setRequest(new_request);
    EXPECT_EQ(context_.getRequest(), new_request);
}

TEST_F(ContextTest, getManager) {
    EXPECT_EQ(&context_.getManager(), &manager_);
}

TEST_F(ContextTest, getClientFd) {
    EXPECT_EQ(context_.getClientFd(), client_fd_);
}

// TODO: add tests
