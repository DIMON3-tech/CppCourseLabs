#cmake_minimum_required(VERSION 3.14)
#
#project(lab8 VERSION 1.0)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
#
#add_subdirectory(tst)
#add_subdirectory(src)
#add_subdirectory(lib)
#
#add_executable(${PROJECT_NAME} tst/src/buffer.cpp tst/src/buffer.h)
#
#add_subdirectory(GoogleTest)

cmake_minimum_required(VERSION 3.12)

project(
        lab8
        VERSION 1.0
        LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(src)

enable_testing()
add_subdirectory(tst)