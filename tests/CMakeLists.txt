enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_tree test_tree.cc)
add_executable(test_trainer test_trainer.cc)

set(DEP_LIBS dabcore libglog libgtest libgmock libglog proto)

target_link_libraries(test_tree ${DEP_LIBS} ${PROTOBUF_LIBRARY})
target_link_libraries(test_trainer ${DEP_LIBS} ${PROTOBUF_LIBRARY})
