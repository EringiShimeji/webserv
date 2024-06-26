#ifndef INTERNAL_HTTP_RESPONSE_WRITER_HPP
#define INTERNAL_HTTP_RESPONSE_WRITER_HPP

#include "status.hpp"
#include "task/io_task_manager.hpp"
#include "task/write_file.hpp"
#include "utils/unit.hpp"
#include "utils/utils.hpp"
#include <sstream>
#include <unistd.h>

template<class T>
class ResponseWriter {
public:
    static const std::string kProtocolVersion;

    ResponseWriter(IOTaskManager &manager, T output, IWriteFileCallback *cb) : manager_(manager), output_(output), cb_(cb), status_code_(kStatusOk) {}

    ~ResponseWriter() {}

    void addBody(const std::string &content) {
        body_ += content;
    }

    template<class V>
    void addHeader(const std::string &key, V value) {
        header_ += generateHeaderLine(key, value);
    }

    void setStatus(HttpStatusCode code) {
        status_code_ = code;
    }

    void send();

private:
    IOTaskManager &manager_;
    T output_;
    IWriteFileCallback *cb_;
    HttpStatusCode status_code_;
    std::string body_;
    std::string header_;

    std::string generateStatusLine() {
        std::string status_code = utils::toString(status_code_);
        std::string status_text = getHttpStatusText(status_code_);

        return kProtocolVersion + " " + status_code + " " + status_text + "\r\n";
    }

    template<class V>
    std::string generateHeaderLine(const std::string &key, V value) {
        return key + ": " + utils::toString(value) + "\r\n";
    }

    template<>
    std::string generateHeaderLine(const std::string &key, const std::string &value) {
        return key + ": " + value + "\r\n";
    }

    std::string generateRawResponseText() {
        std::string content_length = generateHeaderLine("Content-Length", body_.size());

        return generateStatusLine() + content_length + header_ + "\r\n" + body_;
    }
};

template<class T>
const std::string ResponseWriter<T>::kProtocolVersion = "HTTP/1.1";

#endif
