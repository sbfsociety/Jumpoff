cmake_minimum_required(VERSION 3.28)
project(Service LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(Service
        main.cpp
)
target_compile_features(Service PRIVATE cxx_std_17)
target_link_libraries(Service PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)