cmake_minimum_required(VERSION 3.20)
project (Lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    SFML REQUIRED COMPONENTS 
    graphics 
    window 
    system
)

find_package(nlohmann_json REQUIRED)

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp
    src/Matrix.cpp
    src/MainForm.cpp
    src/Parser.cpp
    src/FPSCounter.cpp
    src/Camera.cpp
    src/Logger.cpp
    src/Model.cpp
    src/Scene.cpp
    src/Renderer.cpp
    src/Color.cpp
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    include
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    nlohmann_json::nlohmann_json
)