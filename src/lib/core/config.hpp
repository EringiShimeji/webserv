#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

class Config {};

class ConfigParser {
public:
    static Config parse(const std::string &path);
};


#endif // CONFIG_HPP
