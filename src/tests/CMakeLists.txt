add_executable(bento_kit_session_test session_test.cpp)
target_link_libraries(bento_kit_session_test PRIVATE bento::kit)

add_executable(bento_kit_random_test random_test.cpp)
target_link_libraries(bento_kit_random_test PRIVATE bento::kit)

add_executable(bento_kit_mask_test mask_test.cpp)
target_link_libraries(bento_kit_mask_test PRIVATE bento::kit)

include(CTest)
add_test(NAME bento_kit_session_test COMMAND bento_kit_session_test)
add_test(NAME bento_kit_random_test COMMAND bento_kit_random_test)
add_test(NAME bento_kit_mask_test COMMAND bento_kit_mask_test)
