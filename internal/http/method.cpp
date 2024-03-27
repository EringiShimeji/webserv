#include "method.hpp"

HttpMethod httpMethodFromString(const std::string &method) {
    if (method == "GET") {
        return kMethodGet;
    } else if (method == "POST") {
        return kMethodPost;
    } else if (method == "PUT") {
        return kMethodPut;
    } else if (method == "DELETE") {
        return kMethodDelete;
    } else if (method == "HEAD") {
        return kMethodHead;
    } else if (method == "OPTIONS") {
        return kMethodOptions;
    } else if (method == "TRACE") {
        return kMethodTrace;
    } else if (method == "CONNECT") {
        return kMethodConnect;
    } else if (method == "PATCH") {
        return kMethodPatch;
    } else {
        return kMethodUnknown;
    }
}