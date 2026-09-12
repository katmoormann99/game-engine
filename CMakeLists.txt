cmake_minimum_required(VERSION 3.20)

project(game_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    add_definitions(-DBUILD_MACOS)
    add_definitions(-DGL_GLEXT_PROTOTYPES)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

set(GRAPHICS_SANDBOX
    ${CMAKE_CURRENT_SOURCE_DIR}/../graphics-sandbox
)

add_executable(game_engine
    app/main.cpp

    src/systems/movement_system.cpp
    src/systems/lifetime_system.cpp
    src/systems/sensor_system.cpp
    src/systems/render_system.cpp

    src/spatial/spatial_grid.cpp
    src/simulation/simulation.cpp
    src/rendering/graphics_backend.cpp

    geometry/vector3.cpp
    geometry/point3.cpp
)

target_include_directories(game_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GRAPHICS_SANDBOX}/include
)

target_link_libraries(game_engine
    PRIVATE
        /opt/homebrew/lib/libSDL3.dylib
        "-framework OpenGL"
)