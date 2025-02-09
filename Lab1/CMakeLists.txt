cmake_minimum_required(VERSION 3.20)
project (Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    SFML REQUIRED COMPONENTS 
    graphics 
    window 
    system
)

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp
    src/Matrix.cpp
    src/MainForm.cpp
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
)