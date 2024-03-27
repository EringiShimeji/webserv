#include "write_file.hpp"
#include <unistd.h>

IWriteFileCallback::~IWriteFileCallback() {}

WriteFile::WriteFile(IContext *ctx, const std::string &data_to_write, IWriteFileCallback *cb)
    : IOTask(ctx->getManager(), ctx->getClientFd()), data_to_write_(data_to_write), ctx_(ctx), cb_(cb) {
}

Result<IOTaskResult, std::string> WriteFile::execute() {
    write(fd_, data_to_write_.c_str(), data_to_write_.size());
    cb_->trigger();
    return Ok(kTaskComplete);
}

WriteFile::~WriteFile() {
    delete cb_;
}

WriteFileCallback::WriteFileCallback(IContext *ctx) : ctx_(ctx) {}

Result<types::Unit, std::string> WriteFileCallback::trigger() {
    close(ctx_->getClientFd());
    return Ok(unit);
}
