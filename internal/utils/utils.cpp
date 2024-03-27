#include "utils.hpp"
#include <cerrno>
#include <cstdlib>

std::size_t utils::getStringStreamSize(std::stringstream &ss) {
    ss.seekg(0, std::ios::end);
    std::size_t size = ss.tellg();
    ss.seekg(0, std::ios::beg);
    return size;
}

bool utils::startsWith(const std::string &str, const std::string &prefix) {
    return str.find(prefix) == 0;
}

bool utils::endsWith(const std::string &str, const std::string &suffix) {
    if (str.size() < suffix.size()) {
        return false;
    }
    return str.rfind(suffix) == (str.size() - suffix.size());
}

Result<unsigned long, std::string> utils::stoul(const std::string &str) {
    if (str.empty()) {
        return Err<std::string>("empty string");
    }

    for (size_t i = 0; i < str.size(); i++) {
        if (!std::isdigit(str[i])) {
            return Err<std::string>("contains non-digit character");
        }
    }

    errno = 0;
    unsigned long result = std::strtoul(str.c_str(), NULL, 10);
    if (errno == ERANGE) {
        return Err<std::string>("out of range");
    }

    return Ok(result);
}

// TODO: コピーを避ける
Option<char *> utils::strnstr(const char *haystack, const char *needle, std::size_t len) {
    if (haystack == NULL || needle == NULL) {
        return None;
    }

    std::string haystack_str(haystack, len);
    std::size_t pos = haystack_str.find(needle);
    if (pos == std::string::npos) {
        return None;
    }
    return Some(const_cast<char *>(haystack + pos));
}
