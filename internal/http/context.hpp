#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "request.hpp"
#include "response_writer.hpp"
#include "status.hpp"
#include "task/io_task_manager.hpp"

class IContext {
public:
    virtual ~IContext();
    virtual const Request &getRequest() const = 0;
    virtual void setRequest(const Request &request) = 0;
    virtual void setHeader(const std::string &name, const std::string &value) = 0;
    virtual void text(HttpStatusCode status, const std::string &body) = 0;
    virtual void html(HttpStatusCode status, const std::string &body) = 0;
    virtual void redirect(HttpStatusCode status, const std::string &location) = 0;
    virtual IOTaskManager &getManager() const = 0;
    virtual int getClientFd() const = 0;
};

class Context : public IContext {
public:
    Context(IOTaskManager &manager, int client_fd);
    virtual const Request &getRequest() const;
    virtual void setRequest(const Request &request);
    virtual void setHeader(const std::string &name, const std::string &value);
    virtual void text(HttpStatusCode status, const std::string &body);
    virtual void html(HttpStatusCode status, const std::string &body);
    virtual void redirect(HttpStatusCode status, const std::string &location);
    virtual IOTaskManager &getManager() const;
    virtual int getClientFd() const;

private:
    IOTaskManager &manager_;
    Request request_;
    int client_fd_;
    ResponseWriter<int> writer_;
};

#endif //CONTEXT_HPP
