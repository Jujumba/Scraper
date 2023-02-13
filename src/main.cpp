#include "Http.hpp"
#include "iostream"
int main() {
    Http http;
    std::cout << http.get("www.google.com") << std::endl;
}