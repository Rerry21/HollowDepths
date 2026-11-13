cmake_minimum_required(VERSION 3.28)
project(HollowDepths LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable(HollowDepths
    src/main.cpp
    src/core/Game.cpp
)

target_include_directories(HollowDepths PRIVATE src)
target_link_libraries(HollowDepths PRIVATE SFML::Graphics)

if(MSVC)
    target_compile_options(HollowDepths PRIVATE /W4)
else()
    target_compile_options(HollowDepths PRIVATE -Wall -Wextra -Wpedantic)
endif()