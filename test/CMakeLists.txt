add_executable(test test.cpp)
target_link_libraries(test GTest::gtest ${PROJECT_NAME})
configure_file(vadd.cl ${CMAKE_CURRENT_BINARY_DIR}/vadd.cl COPYONLY)
