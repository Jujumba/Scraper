#ifndef SCRAPER_SCRAPER_HPP
#define SCRAPER_SCRAPER_HPP
#include "Http.hpp"
#include <regex>
#include <vector>
#include <iostream>
namespace scraper {
    class Scraper {
    public:
        Scraper() = default;
        ~Scraper() = default;
        Scraper(Scraper &) = default;
        Scraper(Scraper &&) = default;
        void scrap(const std::string &);
    private:
        Http client{};
        std::regex regex{R"(http://[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}(/\\S*)?)"};
        std::vector<std::string> urls;
    };
}
#endif //SCRAPER_SCRAPER_HPP