cmake_minimum_required(VERSION 3.10)
project(FantasyMultiplayer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

# Add SDL subdirectory
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# ASIO and general includes
include_directories(
    asio/asio/include
)

# Add subdirectories
add_subdirectory(Shared)
add_subdirectory(Client)
add_subdirectory(Server)
