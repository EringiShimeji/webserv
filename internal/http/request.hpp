#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "method.hpp"
#include "utils/option.hpp"
#include <map>
#include <string>

class Request {
public:
    Request();
    explicit Request(HttpMethod method,
                     const std::string &request_target,
                     const std::string &http_version = "HTTP/1.1",
                     const std::map<std::string, std::string> &headers = std::map<std::string, std::string>(),
                     const std::string &body = "");
    Request(const Request &other);
    Request &operator=(const Request &other);
    bool operator==(const Request &rhs) const;

    HttpMethod method() const;
    const std::string &path() const;
    Option<std::string> query(const std::string &key) const;
    const std::string &httpVersion() const;
    Option<std::string> header(const std::string &key) const;
    const std::string &body() const;

private:
    HttpMethod method_;
    std::string request_target_;
    std::string http_version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

#endif
