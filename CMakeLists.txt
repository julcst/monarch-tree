cmake_minimum_required(VERSION 3.26)

project(monarch-tree VERSION 1.0 LANGUAGES CXX)

set(SRC
    src/main.cpp
    src/mainapp.cpp
    src/tree/aabb.cpp
    src/tree/generator.cpp
    src/tree/model.cpp
    src/tree/sdf.cpp
    src/swarm/swarm.cpp
)

# Fetch framework
include(FetchContent)
FetchContent_Declare(
    framework
    #GIT_REPOSITORY https://github.com/julcst/gltemplate/
    URL https://github.com/julcst/gltemplate/archive/v1.5.tar.gz
)
FetchContent_MakeAvailable(framework)

# Add executable
add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE framework)