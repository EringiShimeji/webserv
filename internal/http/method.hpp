#ifndef METHOD_HPP
#define METHOD_HPP

#include <string>

enum HttpMethod {
    kMethodUnknown,
    kMethodGet,
    kMethodPost,
    kMethodPut,
    kMethodDelete,
    kMethodHead,
    kMethodOptions,
    kMethodTrace,
    kMethodConnect,
    kMethodPatch,
};

HttpMethod httpMethodFromString(const std::string &method);

#endif //METHOD_HPP
