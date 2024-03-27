#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "request.hpp"
#include "utils/result.hpp"
#include <vector>

class RequestParser {
public:
    typedef std::pair<std::string, std::string> HeaderField;

    static Result<Request, std::string> parseRequest(const std::string &request_line, const std::vector<std::string> &headers, const std::string &body);
    static Result<std::pair<std::string, std::string>, std::string> parseHeaderFieldLine(const std::string &line);

private:
    // TODO: 分かりにくいのでクラスにする (使用箇所は限定的なので後回し)
    // method, request-target, HTTP-version
    typedef std::pair<std::pair<HttpMethod, std::string>, std::string> RequestLine;

    static Result<RequestLine, std::string> parseRequestLine(const std::string &line);
    static bool isValidFieldName(const std::string &field_name);
    static bool isValidFieldValue(const std::string &field_value);
    static bool isValidHttpVersion(const std::string &http_version);
};

#endif //REQUEST_PARSER_HPP
