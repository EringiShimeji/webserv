#include "request_parser.hpp"
#include "method.hpp"
#include "utils/utils.hpp"

Result<Request, std::string>
RequestParser::parseRequest(const std::string &request_line, const std::vector<std::string> &headers, const std::string &body) {
    // TODO: parse query
    const Result<RequestParser::RequestLine, std::string> parse_request_line_result = parseRequestLine(request_line);
    if (parse_request_line_result.isErr()) {
        return Err(parse_request_line_result.unwrapErr());
    }
    const RequestLine parsed_request_line = parse_request_line_result.unwrap();

    std::map<std::string, std::string> parsed_headers;
    for (std::size_t i = 0; i < headers.size(); i++) {
        const Result<std::pair<std::string, std::string>, std::string> parse_header_field_result = parseHeaderFieldLine(headers[i]);
        if (parse_header_field_result.isErr()) {
            return Err(parse_header_field_result.unwrapErr());
        }
        const HeaderField header_field = parse_header_field_result.unwrap();
        parsed_headers.insert(header_field);
    }

    return Ok(Request(
            parsed_request_line.first.first,  // method
            parsed_request_line.first.second, // request-target
            parsed_request_line.second,       // HTTP-version
            parsed_headers,
            body));
}

// field-line = field-name ":" OWS field-value OWS
Result<std::pair<std::string, std::string>, std::string>
RequestParser::parseHeaderFieldLine(const std::string &line) {
    // : の前後で区切る
    const std::size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        return Err<std::string>("invalid field-line");
    }
    const std::string raw_field_name = line.substr(0, colon_pos);
    const std::string raw_field_value = line.substr(colon_pos + 1);

    // field-name の検証
    if (!isValidFieldName(raw_field_name)) {
        return Err<std::string>("invalid field-name");
    }

    // field-value の検証
    // OWS = *( SP / HTAB ) で trim する
    /* (RFC 9110)
     * A field value does not include leading or trailing whitespace.
     * When a specific version of HTTP allows such whitespace to appear in a message,
     * a field parsing implementation MUST exclude such whitespace
     * prior to evaluating the field value.
     */
    const std::string::size_type begin = raw_field_value.find_first_not_of(" \t");
    const std::string::size_type end = raw_field_value.find_last_not_of(" \t");
    if (begin == std::string::npos || end == std::string::npos) {
        return Err<std::string>("invalid field-value");
    }
    const std::string trimmed_raw_field_value = raw_field_value.substr(begin, end - begin + 1);
    if (!isValidFieldValue(trimmed_raw_field_value)) {
        return Err<std::string>("invalid field-value");
    }

    return Ok(std::make_pair(raw_field_name, trimmed_raw_field_value));
}

// request-line = method SP request-target SP HTTP-version
// NOTE: サーバーは SP 以外にも HTAB, VT, FF, CR を区切りとしてもよい (MAY)
Result<RequestParser::RequestLine, std::string> RequestParser::parseRequestLine(const std::string &line) {
    // 最初と最後のスペースを探し, 3分割する
    const std::string::size_type first_space_pos = line.find(' ');
    const std::string::size_type last_space_pos = line.find_last_of(' ');
    if (first_space_pos == std::string::npos || last_space_pos == std::string::npos) {
        return Err<std::string>("invalid request-line");
    }
    if (first_space_pos == last_space_pos) {
        return Err<std::string>("invalid request-line");
    }
    const std::string raw_method = line.substr(0, first_space_pos);
    const std::string raw_request_target = line.substr(first_space_pos + 1, last_space_pos - first_space_pos - 1);
    const std::string raw_http_version = line.substr(last_space_pos + 1);

    // TODO: request-target の validation
    const HttpMethod method = httpMethodFromString(raw_method);
    if (method == kMethodUnknown) {
        return Err<std::string>("unknown method");
    }
    if (!isValidHttpVersion(raw_http_version)) {
        return Err<std::string>("invalid HTTP-version");
    }

    return Ok(std::make_pair(std::make_pair(method, raw_request_target), raw_http_version));
}

/*
* field-name = token
* token = 1*tchar
* tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*"
*       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
*       / DIGIT / ALPHA ; any VCHAR, except delimiters
*/
bool RequestParser::isValidFieldName(const std::string &field_name) {
    if (field_name.empty()) {
        return false;
    }

    for (size_t i = 0; i < field_name.size(); i++) {
        unsigned char c = field_name[i];
        if (!(std::isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~')) {
            return false;
        }
    }
    return true;
}

/*
* field-value = *field-content
* field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar) ]
* field-vchar = VCHAR / obs-text
* obs-text = %x80-FF
*/
// NOTE: trim 済みであることを仮定すれば, field-value = *field-vchar
bool RequestParser::isValidFieldValue(const std::string &field_value) {
    if (field_value.empty()) {
        return true;
    }

    // 先頭・末尾が SP / HTAB でないことを確認
    if (field_value[0] == ' ' || field_value[0] == '\t' || field_value[field_value.size() - 1] == ' ' || field_value[field_value.size() - 1] == '\t') {
        return false;
    }

    // 最後に *( SP / HTAB / field-vchar ) であることを確認すればよい
    for (size_t i = 0; i < field_value.size(); i++) {
        unsigned char c = field_value[i];
        // TODO: std::isprint で適切か
        if (!std::isprint(c) && !(c >= 0x80 && c <= 0xff)) {
            return false;
        }
    }

    return true;
}

bool RequestParser::isValidHttpVersion(const std::string &http_version) {
    // HTTP/1.1 の形式, 8文字
    if (!(http_version.size() == 8 && utils::startsWith(http_version, "HTTP/"))) {
        return false;
    }
    // バージョンをチェック
    char major = http_version[5];
    char minor = http_version[7];
    return http_version[6] == '.' && std::isdigit(major) && std::isdigit(minor);
}
