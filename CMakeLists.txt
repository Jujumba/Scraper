cmake_minimum_required(VERSION 3.24)
project(Scraper)

set(CMAKE_CXX_STANDARD 23)

add_executable(Scraper src/main.cpp src/Http.cpp src/Http.hpp)
include_directories(lib/asio/include)