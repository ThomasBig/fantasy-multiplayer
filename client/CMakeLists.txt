add_executable(FantasyMultiplayerClient
    main.cpp
    game.cpp
    network.cpp
    renderer.cpp
)

target_include_directories(FantasyMultiplayerClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FantasyMultiplayerClient
    PRIVATE
        Shared
        SDL3::SDL3
)

# Copy all .bmp files after build
file(GLOB BMP_FILES "${CMAKE_SOURCE_DIR}/sprites/*.bmp")
foreach(BMP_FILE ${BMP_FILES})
    add_custom_command(TARGET FantasyMultiplayerClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sprites"
        COMMAND ${CMAKE_COMMAND} -E copy ${BMP_FILE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sprites/")
endforeach()
