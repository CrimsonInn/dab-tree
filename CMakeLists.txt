cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(dabtree)
# Enable ExternalProject CMake module
include(ExternalProject)
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)


ExternalProject_Add(
    glog
    GIT_REPOSITORY "https://github.com/google/glog"
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glog
    # Disable install step
    INSTALL_COMMAND ""
    CMAKE_ARGS "-DWITH_GFLAGS=OFF"
)
ExternalProject_Get_Property(glog source_dir binary_dir)
add_library(libglog IMPORTED STATIC GLOBAL)
add_dependencies(libglog glog)
set_target_properties(libglog PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libglog.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest/googletest/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest/googlemock/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/glog/src/glog-build/"
                    "${CMAKE_CURRENT_BINARY_DIR}/glog/src/glog/src/"
                    )

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread -g -Wall")
find_package (Threads)
find_package(Protobuf REQUIRED)


include_directories(dabtree ${PROJECT_SOURCE_DIR}/include)
#include_directories(dabtree ${CMAKE_CURRENT_BINARY_DIR}/data)


file(GLOB SOURCES
    src/*.cc
    src/core/*.cc
    include/*.h
    data/*.proto
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} libglog ${PROTOBUF_LIBRARY})

#add_library (dabcore STATIC src/core/data.cc src/core/tree.cc src/protobuf/read_proto.cc src/protobuf/write_proto.cc)
add_library (dabcore STATIC src/core/data.cc src/core/tree.cc src/core/matrix.cc)

ADD_SUBDIRECTORY(tests)
#ADD_SUBDIRECTORY(data)
