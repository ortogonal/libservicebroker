cmake_minimum_required(VERSION 3.5)

project(ServiceBroker)

enable_testing()

set(LIBSERVICEBROKER_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(test)
