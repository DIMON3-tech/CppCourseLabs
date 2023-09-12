cmake_minimum_required(VERSION 3.22.1)
project(Forecast VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

if (WIN32) # Install dlls in the same directory as the executable on Windows
   set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)


FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr GIT_TAG 1.10.2)

FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui GIT_TAG v3.0.0)
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json GIT_TAG v3.11.2)

FetchContent_Declare(ftxui
       GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
       GIT_TAG v3.0.0
       )
FetchContent_MakeAvailable(cpr json ftxui)

FetchContent_GetProperties(ftxui)
if (NOT ftxui_POPULATED)
   FetchContent_Populate(ftxui)
   add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

add_subdirectory(Classes)
add_subdirectory(Telegram-bot)

target_include_directories(${PROJECT_NAME} PRIVATE Classes Telegram-bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()