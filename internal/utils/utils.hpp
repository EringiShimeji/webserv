#ifndef UTILS_HPP
#define UTILS_HPP

#include "utils/option.hpp"
#include "utils/result.hpp"
#include <sstream>
#include <string>

namespace utils {
    const unsigned int kKiB = 1024;        // 1 KiB = 1024 bytes
    const unsigned int kMiB = 1024 * kKiB; // 1 MiB = 1024 KiB

    template<class T>
    std::string toString(T value) {
        std::stringstream ss;
        ss << value;

        return ss.str();
    }

    std::size_t getStringStreamSize(std::stringstream &ss);

    bool startsWith(const std::string &str, const std::string &prefix);
    bool endsWith(const std::string &str, const std::string &suffix);

    // 数字以外を含む場合はエラー (std::stoull とは異なる)
    // 先頭・末尾の空白もエラーとする
    // NOTE: C++98 では std::strtoull (cstdlib の関数) を使えないので, utils::strtoull は簡単に実装できない
    Result<unsigned long, std::string> stoul(const std::string &str);

    // 標準ライブラリに含まれないので, 自前で実装
    // haystack, needle が NULL の場合は None を返す
    Option<char *> strnstr(const char *haystack, const char *needle, std::size_t len);
} // namespace utils

#endif //UTILS_HPP
