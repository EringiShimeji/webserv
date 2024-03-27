#include "handler.hpp"
#include "task/write_file.hpp"

IHandler::~IHandler() {}

Result<types::Unit, std::string> Handler::trigger(IContext *ctx) {
    if (ctx == NULL) {
        return Ok(unit);
    }
    ctx->text(kStatusOk, ctx->getRequest().body());
    return Ok(unit);
}
