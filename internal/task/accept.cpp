#include "accept.hpp"
#include "read_request.hpp"
#include "utils/result.hpp"
#include <iostream>
#include <sys/socket.h>

Accept::Accept(IOTaskManager &m, int fd, IAcceptCallback *cb) : IOTask(m, fd), cb_(cb) {}

Accept::~Accept() {
    delete cb_;
}

Result<IOTaskResult, std::string> Accept::execute() {
    // クライアントからの接続を受け入れる
    int client_fd = accept(fd_, NULL, NULL);
    if (client_fd < 0) {
        return Err<std::string>("Error: Accept failed\n");
    }
    std::cout << "Client connected\n";
    cb_->trigger(client_fd);
    return Ok(kTaskSuspend);
}

IAcceptCallback::~IAcceptCallback() {}

AcceptCallback::AcceptCallback(IOTaskManager &manager, IHandler *handler) : manager_(manager), handler_(handler) {}

Result<types::Unit, std::string> AcceptCallback::trigger(int client_fd) {
    new ReadRequest(
            new Context(manager_, client_fd),
            new ReadRequestCallback(handler_),
            new BufferedReader(new FdReader(client_fd), kOwnMove));
    return Ok(unit);
}
