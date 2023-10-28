cmake_minimum_required(VERSION 3.20.0)

find_package(GTest REQUIRED)


set (SUBSCRIBER_TEST_SRC
    tst_subscriber.cpp
    ../src/subscriber/servicesubscriber.cpp
)

add_executable(tst_subscriber ${SUBSCRIBER_TEST_SRC})
add_test(NAME tst_subscriber COMMAND tst_subscriber)
target_include_directories(tst_subscriber
    PRIVATE ../src/subscriber
    PRIVATE ${LIBSERVICEBROKER_HEADERS_DIR}
)
target_link_libraries(tst_subscriber PRIVATE gtest gtest_main pthread)
