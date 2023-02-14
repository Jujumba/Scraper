#ifndef SCRAPER_HTTP_HPP
#define SCRAPER_HTTP_HPP

#define BUFFER_SIZE 1024
#define HTTP_PORT 80
#define REQUIRED_VERSION 0x101
#define HTTP_DELIM1 "\r\n\r\n"
#define HTTP_DELIM2 "\n\r\n\r"
#define HTTP_PREFIX "http://"
#define HTTPS_PREFIX "https://"
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

typedef unsigned long long ulong;
class Http {
public:
    std::string get(const std::string&, bool = false);
private:
    void connect(SOCKET*, std::string&);
    std::string read(std::string&);
    void parse_url(std::string&, std::string&, std::string&);
    ulong get_header_length(const std::string&);
    std::string strhash(const std::string&);
};


#endif //SCRAPER_HTTP_HPP
