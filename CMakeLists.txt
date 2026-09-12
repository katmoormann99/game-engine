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

add_executable(
    game_engine

    app/main.cpp

    src/systems/movement_system.cpp
    src/systems/lifetime_system.cpp
    src/systems/sensor_system.cpp
    src/systems/render_system.cpp
    src/systems/collision_system.cpp
    src/systems/weapon_system.cpp

    src/spatial/spatial_grid.cpp

    src/simulation/simulation.cpp

    src/rendering/graphics_backend.cpp
    src/rendering/obj_loader.cpp

    src/factory/entity_factory.cpp

    geometry/vector3.cpp
    geometry/point3.cpp
    geometry/matrix.cpp
    geometry/geometry.cpp
)

target_include_directories(
    game_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GRAPHICS_SANDBOX}/include

        # Homebrew SDL3 headers:
        /opt/homebrew/include
)

target_link_libraries(
    game_engine
    PRIVATE
        /opt/homebrew/lib/libSDL3.dylib
        "-framework OpenGL"
)

set(SHADER_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

set(SHADER_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

add_custom_command(
    OUTPUT
        ${SHADER_BINARY_DIR}/basic.vert
        ${SHADER_BINARY_DIR}/basic.frag

    COMMAND
        ${CMAKE_COMMAND} -E make_directory
        ${SHADER_BINARY_DIR}

    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_SOURCE_DIR}/basic.vert
        ${SHADER_BINARY_DIR}/basic.vert

    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_SOURCE_DIR}/basic.frag
        ${SHADER_BINARY_DIR}/basic.frag

    DEPENDS
        ${SHADER_SOURCE_DIR}/basic.vert
        ${SHADER_SOURCE_DIR}/basic.frag

    COMMENT "Updating shaders"
)

add_custom_target(
    copy_shaders ALL
    DEPENDS
        ${SHADER_BINARY_DIR}/basic.vert
        ${SHADER_BINARY_DIR}/basic.frag
)

add_dependencies(
    game_engine
    copy_shaders
)