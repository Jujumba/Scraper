#include "Scraper.hpp"
void scraper::Scraper::scrap(const std::string& root) {
    std::string content = client.get(root);
    ulong start = 0, end = 0;
    if (std::find(urls.begin(), urls.end(), root) != urls.end()) return;
    while (end < content.size()) {
        start = content.find('\"', end);
        end = content.find('\"', start + 1);
        const std::string& url = content.substr(start + 1, end - start - 1);
        if (std::count(urls.begin(), urls.end(), url) == 0 && (url.starts_with(HTTP_PREFIX) || url.starts_with(HTTPS_PREFIX))) {
            urls.push_back(url);
            std::cout << url << std::endl;
            scrap(url);
        }
        if (start == end) break;
    }
}