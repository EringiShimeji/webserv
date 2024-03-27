#ifndef WRITEFILE_HPP
#define WRITEFILE_HPP

#include "callback_interface.hpp"
#include "http/context.hpp"
#include "io_task_manager.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class IWriteFileCallback {
public:
    virtual ~IWriteFileCallback();
    virtual Result<types::Unit, std::string> trigger() = 0;
};

class WriteFileCallback : public IWriteFileCallback {
public:
    explicit WriteFileCallback(IContext *ctx);
    Result<types::Unit, std::string> trigger();

private:
    IContext *ctx_;
};

class WriteFile : public IOTask {
public:
    WriteFile(IContext *ctx, const std::string &data_to_write, IWriteFileCallback *cb);
    ~WriteFile();
    virtual Result<IOTaskResult, std::string> execute();

private:
    const std::string data_to_write_;
    IContext *ctx_;
    IWriteFileCallback *cb_;
};

#endif
