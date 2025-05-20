cmake_minimum_required(VERSION 3.20)
project (Lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML REQUIRED COMPONENTS graphics window system)
find_package(TinyGLTF REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(
    ${PROJECT_NAME} 
    ${SRC_FILES}
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
    glm::glm
    TinyGLTF::TinyGLTF
)