#include "context.hpp"

IContext::~IContext() {}

Context::Context(IOTaskManager &manager, int client_fd)
    : manager_(manager), client_fd_(client_fd), writer_(manager, client_fd) {}

const Request &Context::getRequest() const {
    return request_;
}

void Context::setRequest(const Request &request) {
    request_ = request;
}

void Context::setHeader(const std::string &name, const std::string &value) {
    writer_.addHeader(name, value);
}

void Context::text(HttpStatusCode status, const std::string &body) {
    writer_.setStatus(status);
    writer_.addHeader("Content-Type", "text/plain");
    writer_.addBody(body);
    writer_.send();
}

void Context::html(HttpStatusCode status, const std::string &body) {
    writer_.setStatus(status);
    writer_.addHeader("Content-Type", "text/html");
    writer_.addBody(body);
    writer_.send();
}

void Context::redirect(HttpStatusCode status, const std::string &location) {
    writer_.setStatus(status);
    writer_.addHeader("Location", location);
    writer_.send();
}

IOTaskManager &Context::getManager() const {
    return manager_;
}

int Context::getClientFd() const {
    return client_fd_;
}
