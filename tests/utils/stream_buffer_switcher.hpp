#ifndef TESTS_UTILS_STREAM_BUFFER_SWITCHER_HPP
#define TESTS_UTILS_STREAM_BUFFER_SWITCHER_HPP

#include <ostream>

class StreamBufferSwitcher {
public:
    StreamBufferSwitcher(std::ostream &os, std::streambuf *new_buffer);
    ~StreamBufferSwitcher();
    StreamBufferSwitcher(const StreamBufferSwitcher &) = delete;
    StreamBufferSwitcher &operator=(const StreamBufferSwitcher &) = delete;

private:
    std::ostream &os_;
    std::streambuf *old_buffer_;
};

#endif //TESTS_UTILS_STREAM_BUFFER_SWITCHER_HPP
