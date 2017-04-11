enable_testing()
#find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_tree test_tree.cc)
target_link_libraries(test_tree
    libgtest
    libgmock
)
