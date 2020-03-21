add_executable(ComPtr_test ComPtr_test.cpp)
target_link_libraries(ComPtr_test gtest_main gmock_main)
add_test(NAME ComPtr_test COMMAND ComPtr_test)

add_executable(IUnknownImpl_test IUnknownImpl_test.cpp)
target_link_libraries(IUnknownImpl_test gtest_main gmock_main)
add_test(NAME IUnknownImpl_test COMMAND IUnknownImpl_test)
