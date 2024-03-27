#ifndef READREQUEST_HPP
#define READREQUEST_HPP

#include "handler/handler.hpp"
#include "http/context.hpp"
#include "io/reader.hpp"
#include "io_task.hpp"
#include "utils/result.hpp"
#include "utils/unit.hpp"
#include "utils/utils.hpp"
#include <sstream>

class IReadRequestCallback {
public:
    virtual ~IReadRequestCallback();
    virtual Result<types::Unit, std::string> trigger(IContext *ctx) = 0;
};

class ReadRequestCallback : public IReadRequestCallback {
public:
    explicit ReadRequestCallback(IHandler *handler);
    virtual Result<types::Unit, std::string> trigger(IContext *ctx);

private:
    IHandler *handler_;
};

class ReadRequest : public IOTask {
public:
    ReadRequest(IContext *ctx, IReadRequestCallback *cb, IBufferedReader *reader);
    ~ReadRequest();
    virtual Result<IOTaskResult, std::string> execute();

private:
    IContext *ctx_;
    IReadRequestCallback *cb_;
    IBufferedReader *reader_;
    std::vector<std::string> headers_;
};

#endif
