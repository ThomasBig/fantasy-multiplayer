# Create Shared as a static library
add_library(Shared
    player.cpp
    players.cpp
)

target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
