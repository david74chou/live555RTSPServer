cmake_minimum_required(VERSION 2.6)
project(live555RTSPServer)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(LIVE555 REQUIRED live555)
if(LIVE555_FOUND)
    include_directories(${LIVE555_INCLUDE_DIRS})
    link_directories(${LIVE555_LIBRARY_DIRS})
endif()

add_executable(live555rtspserver main.cpp)
target_link_libraries(live555rtspserver
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIVE555_LIBRARIES})