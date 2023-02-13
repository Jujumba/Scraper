#ifndef SCRAPER_HTTP_HPP
#define SCRAPER_HTTP_HPP

#define BUFFER_SIZE 1024
#define HTTP_PORT 80
#define REQUIRED_VERSION 0x101
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <string>
#include <fstream>

typedef unsigned long ulong;
class Http {
public:
    std::string get(const std::string&, bool = false);
private:
    void connect(SOCKET*, std::string&);
    std::string read(std::string&);
    void parse_url(std::string&, std::string&, std::string&, std::string&);
    ulong get_header_length(const std::string&);
};


#endif //SCRAPER_HTTP_HPP
