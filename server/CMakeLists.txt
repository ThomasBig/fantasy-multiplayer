add_executable(FantasyMultiplayerServer
    main.cpp
    server.cpp
)

target_include_directories(FantasyMultiplayerServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FantasyMultiplayerServer
    PRIVATE
        Shared
)
