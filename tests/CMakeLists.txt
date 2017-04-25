enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_tree test_tree.cc)
target_link_libraries(test_tree
    dabcore
    libglog
    libgtest
    libgmock
    libglog
    proto
    ${PROTOBUF_LIBRARY}
)
