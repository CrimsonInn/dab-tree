project(dabtree)
cmake_minimum_required(VERSION 3.5)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
find_package (Threads)
find_package(Protobuf REQUIRED)

include_directories(dabtree ${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES
    src/*.cc
    core/*.cc
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_executable(${PROJECT_NAME} ${SOURCES})
