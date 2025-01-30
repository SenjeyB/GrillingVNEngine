cmake_minimum_required(VERSION 3.28)
project(VNEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0)
FetchContent_MakeAvailable(yaml-cpp)

add_executable(main
    src/main.cpp
    src/Game.cpp
    src/Dialog.cpp
    src/Character.cpp
    src/Scene.cpp
    src/SceneManager.cpp
    src/ScriptParser.cpp
    src/ScriptedScene.cpp
    src/FontGenerator.cpp
    src/MusicManager.cpp
)

target_link_libraries(main PRIVATE 
    SFML::Graphics 
    SFML::Audio
    yaml-cpp
)
