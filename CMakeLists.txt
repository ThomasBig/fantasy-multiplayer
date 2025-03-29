cmake_minimum_required(VERSION 3.10)
project(FantasyChat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(WIN32)
    # Windows 10 minimum requirement
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Add the include directory for ASIO and your headers
include_directories(
    asio/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}  # for shared.hpp if it's in the root directory
)

# Shared source
set(SHARED_SRC shared.cpp)

# Build client executable
add_executable(FantasyChatClient client.cpp ${SHARED_SRC})

# Build server executable
add_executable(FantasyChatServer server.cpp ${SHARED_SRC})

target_link_libraries(FantasyChatClient PRIVATE SDL3::SDL3)
target_link_libraries(FantasyChatServer PRIVATE SDL3::SDL3)
