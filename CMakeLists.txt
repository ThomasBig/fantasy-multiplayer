cmake_minimum_required(VERSION 3.10)
project(FantasyChat)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    # Windows 10 minimum requirement
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

# Add the include directory for ASIO
include_directories(asio/asio/include)

add_executable(FantasyChat main.cpp)