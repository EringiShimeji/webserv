#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "http/status.hpp"
#include "utils/result.hpp"
#include "utils/utils.hpp"
#include "virtual_server_config.hpp"
#include <map>
#include <string>

class Config {
public:
    Config();
    explicit Config(
            const std::vector<VirtualServerConfig> &virtual_servers,
            const std::map<HttpStatusCode, std::string> &error_pages = std::map<HttpStatusCode, std::string>(),
            unsigned int client_max_body_size = kDefaultClientMaxBodySize);
    ~Config();
    Config(const Config &other);
    Config &operator=(const Config &other);

    unsigned int getClientMaxBodySize() const;
    const std::vector<VirtualServerConfig> &getVirtualServers() const;
    // There should be no need for the map itself, so no getter has been provided
    const std::string &getErrorPage(HttpStatusCode status_code);
    static Result<Config, std::string> parseConfigFile(const std::string &path);

    static const std::string kDefaultPath;

private:
    // Same as nginx default
    // refs: https://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size
    static const unsigned int kDefaultClientMaxBodySize = utils::kMiB;

    // Max body size of client request (bytes)
    unsigned int client_max_body_size_;
    // Config consists of virtual server configs
    std::vector<VirtualServerConfig> virtual_servers_;
    // Similar to error_page directive in nginx
    // Value is HTML of error page
    // refs: https://nginx.org/en/docs/http/ngx_http_core_module.html#error_page
    std::map<HttpStatusCode, std::string> error_pages_;

    void setDefaultErrorPage(HttpStatusCode code);
};

#endif //CONFIG_HPP
